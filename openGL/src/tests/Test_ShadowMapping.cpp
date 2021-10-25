#include "Test_ShadowMapping.h"
#include "VertexBufferLayout.h"
#include "imgui/imgui.h"

test::Test_ShadowMapping::Test_ShadowMapping()
	:shader("res/shaders/shaderv.vert", "res/shaders/shaderf.frag"),
	lightShader("res/shaders/lightShaderv.vert", "res/shaders/lightShaderf.frag"),
	screenShader("res/shaders/screenShaderv.vert", "res/shaders/screenShaderf.frag"),
	depthMapShader("res/shaders/depthMap/depthMapv.vert", "res/shaders/depthMap/depthMapf.frag"),
	basicShader("res/shaders/basicv.vert", "res/shaders/basicf.frag")
{
	//init arrays and buffers
	VertexBuffer vb(vertices, (unsigned int)sizeof(vertices));
	VertexBufferLayout layout;

	//position
	layout.Push<float>(3);

	//normals
	layout.Push<float>(3);

	//textCoords
	layout.Push<float>(2);
	va.AddBuffer(vb, layout);
	va.Unbind();

	//lightVertexArray
	VertexBufferLayout lightLayout;
	//setting light vertex attributes
	lightLayout.Push<float>(3);
	lightLayout.strideOverride(5, GL_FLOAT);
	lightVA.AddBuffer(vb, lightLayout);
	lightVA.Unbind();

	//quadva
	quadVA.Bind();
	VertexBuffer quadVB(quadVerts, (unsigned int)sizeof(quadVerts));
	VertexBufferLayout quadLayout;
	quadLayout.Push<float>(2);
	quadLayout.Push<float>(2);
	quadVA.AddBuffer(quadVB, quadLayout);
	quadVA.Unbind();

	//planeVA
	VertexBuffer planeVB(planeVerts, (unsigned int)sizeof(planeVerts));

	planeVA.Bind();
	VertexBufferLayout planeLayout;
	planeLayout.Push<float>(3);
	planeLayout.Push<float>(3);
	planeLayout.Push<float>(2);
	planeVA.AddBuffer(planeVB, planeLayout);
	planeVA.Unbind();

	//shadowmap
	depthMapFB.GenTextureBuffer(depthMap, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT, SHADOW_WIDTH, SHADOW_HEIGHT);
	depthMapFB.Unbind();

	//stbi_set_flip_vertically_on_load(true);

	//wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	shader.Bind();
	shader.SetUniform1i("shadowMap", 0);
	shader.Unbind();

	//blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

test::Test_ShadowMapping::~Test_ShadowMapping()
{
}

void test::Test_ShadowMapping::OnUpdate(float deltaTime, GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float lightSpeed = 5.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.ProcessMovement(cam.FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.ProcessMovement(cam.BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.ProcessMovement(cam.RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.ProcessMovement(cam.LEFT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		pointLightPosition += glm::cross(glm::vec3(0.0f, 1.0f, 0.0f) ,cam.GetCamRight())  * lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		pointLightPosition -= glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cam.GetCamRight()) * lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		pointLightPosition -= cam.GetCamRight() * lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		pointLightPosition += cam.GetCamRight() * lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		pointLightPosition.y += lightSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		pointLightPosition.y -= lightSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS && inputFlag == 0)
	{
		switch (cameraLock) {
		case 0:
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			cameraLock = 1;
			mouseCtrl = 1;
			break;

		case 1:
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			cameraLock = 0;
			mouseCtrl = 0;
			cam.SetMouseFirst();
			break;
		}

		inputFlag = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_RELEASE)
		inputFlag = 0;
}

void test::Test_ShadowMapping::OnRender()
{
	//rendering to shadowmap
	//lightPosition = cam.GetCamPosition() + glm::vec3(0.0f, 4.0f, 0.0f);

	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, shadowNear, shadowFar);
	lightView = glm::lookAt(lightPosition,
							lightPosition + dirLightDirection, 
							glm::vec3(0.0f, 1.0f, 0.0f));

	lightVP = lightProjection * lightView;

	depthMapShader.Bind();
	depthMapShader.SetUniformMatrix4fv("lightVP", lightVP);

	//Framebuffer: ShadowMap
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	depthMapFB.Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	//ground
	model = glm::mat4(1.0f);
	depthMapShader.SetUniformMatrix4fv("model", model);
	planeVA.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//cubes
	va.Bind();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	depthMapShader.SetUniformMatrix4fv("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.5f));
	depthMapShader.SetUniformMatrix4fv("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.25));
	depthMapShader.SetUniformMatrix4fv("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4(1.0f);
	model = glm::translate(model, pointLightPosition);
	model = glm::scale(model, glm::vec3(0.2f));
	depthMapShader.SetUniformMatrix4fv("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glCullFace(GL_BACK);

	//Framebuffer: Backbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(0.3f, 0.2f, 0.3f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	projection = glm::perspective(glm::radians(cam.GetFov()), (float) WINDOW_WIDTH / WINDOW_HEIGHT, near, far);
	view = cam.GetViewMatrix(); //<-- add translation back to camera
	vp = projection * view;

	Debug::SetViewProj(vp);

	//render pointlight
	lightShader.Bind();

	model = glm::mat4(1.0f);
	model = glm::translate(model, pointLightPosition);
	model = glm::scale(model, glm::vec3(0.2f));
	mvp = projection * view * model;
	lightShader.SetUniformMatrix4fv("mvp", mvp);
	lightShader.SetUniform3fv("lightColor", pointLightColor);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	renderer.DrawArrays(lightVA, lightShader);
	
	model = glm::mat4(1.0f);

	shader.Bind();

	shader.SetUniformMatrix4fv("lightVP", lightVP);
	shader.SetUniform1f("near", near);
	shader.SetUniform1f("far", far);

	shader.SetUniform3fv("viewPosition", cam.GetCamPosition());

	shader.SetUniform3fv("dirLight.color", dirLightColor);
	shader.SetUniform3fv("dirLight.direction", dirLightDirection);
	shader.SetUniform3fv("dirLight.ambient", dirLightAmbient);
	shader.SetUniform3fv("dirLight.diffuse", dirLightDiffuse);
	shader.SetUniform3fv("dirLight.specular", dirLightSpecular);

	shader.SetUniform3fv("pointLight.color", pointLightColor);
	shader.SetUniform3fv("pointLight.position", pointLightPosition);
	shader.SetUniform3fv("pointLight.ambient", pointLightAmbient);
	shader.SetUniform3fv("pointLight.diffuse", pointLightDiffuse);
	shader.SetUniform3fv("pointLight.specular", pointLightSpecular);
	shader.SetUniform3fv("pointLight.atten", attenuationParams);

	shader.SetUniform1i("halfkernelWidth", halfkernelWidth);

	shader.SetUniform1f("mat.shininess", matShininess);

	//render ground
	model = glm::mat4(1.0f);
	shader.SetUniformMatrix4fv("model", model);
	mvp = projection * view * model;
	shader.SetUniformMatrix4fv("mvp", mvp);
	normal = glm::transpose(glm::inverse(model));
	shader.SetUniformMatrix4fv("normalMatrix", normal);
	shader.SetUniform4fv("color", glm::vec4(0.5f));
	planeVA.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//render cubes
	shader.SetUniform4fv("color", glm::vec4(0.5f, 0.0f, 0.0f, 1.0f));
	va.Bind();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.SetUniformMatrix4fv("model", model);
	mvp = projection * view * model;
	shader.SetUniformMatrix4fv("mvp", mvp);
	normal = glm::transpose(glm::inverse(model));
	shader.SetUniformMatrix4fv("normalMatrix", normal);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.SetUniformMatrix4fv("model", model);
	mvp = projection * view * model;
	shader.SetUniformMatrix4fv("mvp", mvp);
	normal = glm::transpose(glm::inverse(model));
	shader.SetUniformMatrix4fv("normalMatrix", normal);
	va.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.25));
	shader.SetUniformMatrix4fv("model", model);
	mvp = projection * view * model;
	shader.SetUniformMatrix4fv("mvp", mvp);
	normal = glm::transpose(glm::inverse(model));
	shader.SetUniformMatrix4fv("normalMatrix", normal);
	va.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	Debug::DrawOrthProj(lightVP);
	//Debug::DrawLine(glm::vec3(0, 0, 0), pointLightPosition);

	/*screenShader.Bind();
	screenShader.SetUniform1i("screenTexture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	
	quadVA.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);*/
}

void test::Test_ShadowMapping::OnImGuiRender()
{
	//imgui
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("color");

		ImGui::SliderFloat("near", &near, 0.0f, 1.0f);
		ImGui::SliderFloat("far", &far, 50.0f, 100.0f);
		ImGui::SliderFloat3("light direction", (float*)&dirLightDirection, -1.0f, 1.0f);
		ImGui::SliderInt("half kernel width", &halfkernelWidth, 0, 10);
		//ImGui::InputInt("Kuwahara Radius", &kuwahara_radius);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();
	}
}

void test::Test_ShadowMapping::CursorInput(double xPos, double yPos)
{
	if (mouseCtrl == 1)
		cam.ProcessMouseInput(xPos, yPos);
}

void test::Test_ShadowMapping::ScrollInput(double xOffset, double yOffset)
{
	cam.ProcessScroll(xOffset, yOffset);
}