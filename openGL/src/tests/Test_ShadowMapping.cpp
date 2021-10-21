#include "Test_ShadowMapping.h"
#include "VertexBufferLayout.h"
#include "imgui/imgui.h"

test::Test_ShadowMapping::Test_ShadowMapping()
	:shader("res/shaders/shaderv.vert", "res/shaders/shaderf.frag"),
	lightShader("res/shaders/lightShaderv.vert", "res/shaders/lightShaderf.frag"),
	screenShader("res/shaders/screenShaderv.vert", "res/shaders/screenShaderf.frag"),
	depthMapShader("res/shaders/depthMap/depthMapv.vert", "res/shaders/depthMap/depthMapf.frag")
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
	depthMapFB.GenTextureBuffer(depthMap, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
	//glReadBuffer(GL_NONE);
	//glDrawBuffer(GL_NONE);
	depthMapFB.Unbind();

	stbi_set_flip_vertically_on_load(true);

	//wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//backface culling
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

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
		pointLightPosition += cam.GetCamFront() * lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		pointLightPosition -= cam.GetCamFront() * lightSpeed;
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
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, shadowNear, shadowFar);
	lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), 
							glm::vec3(0.0f, 0.0f, 0.0f), 
							glm::vec3(0.0f, 1.0f, 0.0f));

	lightVP = lightProjection * lightView;

	depthMapShader.Bind();
	depthMapShader.SetUniformMatrix4fv("lightVP", lightVP);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	depthMapFB.Bind();
	glClear(GL_DEPTH_BUFFER_BIT);

	//ground
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
	depthMapShader.SetUniformMatrix4fv("model", model);
	planeVA.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//cube
	model = glm::mat4(1.0f);
	model = glm::translate(model, cubepositions[2]);
	depthMapShader.SetUniformMatrix4fv("model", model);
	va.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//rendering to backbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearColor(0.3f, 0.2f, 0.3f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	screenShader.Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	quadVA.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//projection = glm::perspective(glm::radians(cam.GetFov()), 800.0f / 600.0f, near, far);
	//view = cam.GetViewMatrix(); //<-- add translation back to camera

	//lightShader.Bind();

	//model = glm::mat4(1.0f);
	//model = glm::translate(model, pointLightPosition);
	//model = glm::scale(model, glm::vec3(0.2f));
	//mvp = projection * view * model;
	//lightShader.SetUniformMatrix4fv("mvp", mvp);
	//lightShader.SetUniform3fv("lightColor", pointLightColor);

	//renderer.DrawArrays(lightVA, lightShader);
	//
	////render cubes
	//model = glm::mat4(1.0f);

	//shader.Bind();

	//shader.SetUniform1f("near", near);
	//shader.SetUniform1f("far", far);

	//shader.SetUniform3fv("viewPosition", cam.GetCamPosition());

	//shader.SetUniform3fv("dirLight.color", dirLightColor);
	//shader.SetUniform3fv("dirLight.direction", dirLightDirection);
	//shader.SetUniform3fv("dirLight.ambient", dirLightAmbient);
	//shader.SetUniform3fv("dirLight.diffuse", dirLightDiffuse);
	//shader.SetUniform3fv("dirLight.specular", dirLightSpecular);

	//shader.SetUniform3fv("pointLight.color", pointLightColor);
	//shader.SetUniform3fv("pointLight.position", pointLightPosition);
	//shader.SetUniform3fv("pointLight.ambient", pointLightAmbient);
	//shader.SetUniform3fv("pointLight.diffuse", pointLightDiffuse);
	//shader.SetUniform3fv("pointLight.specular", pointLightSpecular);
	//shader.SetUniform3fv("pointLight.atten", attenuationParams);

	//shader.SetUniform1f("mat.shininess", matShininess);

	//model = glm::translate(model, cubepositions[2]);
	//mvp = projection * view * model;
	//shader.SetUniformMatrix4fv("mvp", mvp);
	//shader.SetUniformMatrix4fv("model", model);

	//shader.SetUniform4fv("color", color);

	//normal = glm::transpose(glm::inverse(model));
	//shader.SetUniformMatrix4fv("normalMatrix", normal);

	//renderer.DrawArrays(va, shader);

	////render ground
	//model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
	//mvp = projection * view * model;
	//shader.SetUniformMatrix4fv("mvp", mvp);
	//shader.SetUniformMatrix4fv("model", model);

	//glDisable(GL_CULL_FACE);
	//shader.SetUniform4fv("color", glm::vec4(0.5f));

	//normal = glm::transpose(glm::inverse(model));
	//shader.SetUniformMatrix4fv("normalMatrix", normal);
	//
	//planeVA.Bind();
	//glDrawArrays(GL_TRIANGLES, 0, 6);
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
		ImGui::ColorEdit3("color", (float*)&color);
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