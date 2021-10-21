#include "Test_MSTransparency.h"
#include "VertexBufferLayout.h"
#include "imgui/imgui.h"

test::Test_MSTransparency::Test_MSTransparency()
	:shader("res/shaders/shaderv.vert", "res/shaders/shaderf.frag"),
	outlineShader("res/shaders/lightShaderv.vert", "res/shaders/stencilOutline.frag"),
	lightShader("res/shaders/lightShaderv.vert", "res/shaders/lightShaderf.frag"),
	cubeMapShader("res/shaders/cubemapv.vert", "res/shaders/cubemapf.frag"),
	compositeShader("res/shaders/compositeShaderv.vert", "res/shaders/compositeShaderf.frag"),
	screenShader("res/shaders/screenShaderv.vert", "res/shaders/screenShaderf.frag"),
	experimental("res/shaders/shaderv.vert", "res/shaders/experimentalf.frag")
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

	//render target for framebuffers
	quadVA.Bind();
	VertexBuffer quadVB(quadVerts, (unsigned int)sizeof(quadVerts));
	VertexBufferLayout quadLayout;
	quadLayout.Push<float>(2);
	quadLayout.Push<float>(2);
	quadVA.AddBuffer(quadVB, quadLayout);
	quadVA.Unbind();

	//setting up framebuffers for transparency

	opaqueFB.GenTextureBufferMS(opaqueBuffer, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, GL_COLOR_ATTACHMENT0, 4);
	opaqueFB.GenTextureBufferMS(depthBuffer, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_STENCIL_ATTACHMENT, 4);

	//transparentFB has 3 attachments: 2 draw buffers for color, 1 depth buffer for... well, depth lmao
	transparentFB.GenTextureBufferMS(accumTexture, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, GL_COLOR_ATTACHMENT0, 4);
	transparentFB.GenTextureBufferMS(revealTexture, GL_R8, GL_RED, GL_FLOAT, GL_COLOR_ATTACHMENT1, 4);
	transparentFB.attachTextureBufferMS(GL_DEPTH_STENCIL_ATTACHMENT, depthBuffer); //<-the transparent framebuffer also uses the depth texture as it's order independent

	//explicitly telling opengl we have 2 draw buffers for the transparent framebuffer
	transparentFB.Bind();
	const GLenum transparentDrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, transparentDrawBuffers);
	transparentFB.Unbind();

	opaqueScreenFB.GenTextureBuffer(opaqueScreenTex, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, GL_COLOR_ATTACHMENT0);
	transparentScreenFB.GenTextureBuffer(accumScreenTex, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, GL_COLOR_ATTACHMENT0);
	transparentScreenFB.GenTextureBuffer(revealScreenTex, GL_R8, GL_RED, GL_FLOAT, GL_COLOR_ATTACHMENT1);

	skybox.Load(skyboxFaces);

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

test::Test_MSTransparency::~Test_MSTransparency()
{
}

void test::Test_MSTransparency::OnUpdate(float deltaTime, GLFWwindow* window)
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

void test::Test_MSTransparency::OnRender()
{
	//set depth testing for opaque rendering
	glEnable(GL_DEPTH_TEST); //<-- keep track
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE); //<-- start writing to depth buffer
	glDisable(GL_BLEND);
	glClearColor(0.3f, 0.2f, 0.3f, 1.0f);

	//set projection matrix
	projection = glm::perspective(glm::radians(cam.GetFov()), 800.0f / 600.0f, near, far);

	//bind opaque framebuffer
	opaqueFB.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//opaque rendering begins
	glStencilMask(0x00);

	glEnable(GL_CULL_FACE); //<- Enable face culling for opaque rendering
	glCullFace(GL_BACK);

	//cubemap
	view = glm::mat4(glm::mat3(cam.GetViewMatrix())); //<-- remove translation due to camera

	vp = projection * view;
	cubeMapShader.Bind();
	cubeMapShader.SetUniformMatrix4fv("vp", vp);

	skybox.Render(cubeMapShader);

	view = cam.GetViewMatrix(); //<-- add translation back to camera

	//render lights
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF); //<-- Start writing to stencil buffer

	lightShader.Bind();

	model = glm::mat4(1.0f);
	model = glm::translate(model, pointLightPosition);
	model = glm::scale(model, glm::vec3(0.2f));
	mvp = projection * view * model;
	lightShader.SetUniformMatrix4fv("mvp", mvp);
	lightShader.SetUniform3fv("lightColor", pointLightColor);

	renderer.DrawArrays(lightVA, lightShader);

	//light stencil
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00); //<-- stop writing to stencil buffer
	glDisable(GL_DEPTH_TEST);

	model = glm::scale(model, glm::vec3(1.1f));
	mvp = projection * view * model;
	outlineShader.Bind();
	outlineShader.SetUniformMatrix4fv("mvp", mvp);
	renderer.DrawArrays(lightVA, outlineShader);

	glEnable(GL_DEPTH_TEST);
	glStencilMask(0x00);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);

	//render cubes
	model = glm::mat4(1.0f);

	shader.Bind();

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

	shader.SetUniform1f("mat.shininess", matShininess);

	model = glm::translate(model, cubepositions[2]);
	mvp = projection * view * model;
	shader.SetUniformMatrix4fv("mvp", mvp);
	shader.SetUniformMatrix4fv("model", model);

	//color = glm::vec4(0.6f / 3, 0.4f / 3, 0.2f, 1.0f);
	//color = glm::vec4(0.1f, 0.0f, 0.0f, 1.0f);
	shader.SetUniform4fv("color", color);

	normal = glm::transpose(glm::inverse(model));
	shader.SetUniformMatrix4fv("normalMatrix", normal);

	renderer.DrawArrays(va, shader);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, opaqueFB.getID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, opaqueScreenFB.getID());
	glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	//prepare for switching framebuffers
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunci(0, GL_ONE, GL_ONE);
	glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
	glBlendEquation(GL_FUNC_ADD);

	transparentFB.Bind();

	glClearBufferfv(GL_COLOR, 0, &zeroFillerVec[0]);
	glClearBufferfv(GL_COLOR, 1, &oneFillerVec[0]);

	model = glm::mat4(1.0f);

	experimental.Bind();

	experimental.SetUniform1f("near", near);
	experimental.SetUniform1f("far", far);

	experimental.SetUniform3fv("viewPosition", cam.GetCamPosition());

	experimental.SetUniform3fv("dirLight.color", dirLightColor);
	experimental.SetUniform3fv("dirLight.direction", dirLightDirection);
	experimental.SetUniform3fv("dirLight.ambient", dirLightAmbient);
	experimental.SetUniform3fv("dirLight.diffuse", dirLightDiffuse);
	experimental.SetUniform3fv("dirLight.specular", dirLightSpecular);

	experimental.SetUniform3fv("pointLight.color", pointLightColor);
	experimental.SetUniform3fv("pointLight.position", pointLightPosition);
	experimental.SetUniform3fv("pointLight.ambient", pointLightAmbient);
	experimental.SetUniform3fv("pointLight.diffuse", pointLightDiffuse);
	experimental.SetUniform3fv("pointLight.specular", pointLightSpecular);
	experimental.SetUniform3fv("pointLight.atten", attenuationParams);

	experimental.SetUniform1f("mat.shininess", matShininess);

	//transparent rendering begins
	glDisable(GL_CULL_FACE); //<-disable face culling fro transparent rendering

	for (int i = 0; i < 5; i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, cubepositions[i]);

		glm::vec4 tColor = glm::vec4(0.6f / (i + 1.0f), 0.4f / (i + 1.0f), 0.1f * i, 0.4f);

		mvp = projection * view * model;
		experimental.SetUniformMatrix4fv("mvp", mvp);
		experimental.SetUniformMatrix4fv("model", model);

		experimental.SetUniform4fv("color", tColor);
		normal = glm::transpose(glm::inverse(model));
		experimental.SetUniformMatrix4fv("normalMatrix", normal);

		if (i != 2) renderer.DrawArrays(va, experimental);
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, transparentFB.getID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, transparentScreenFB.getID());

	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glDrawBuffer(GL_COLOR_ATTACHMENT1);
	glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glDepthFunc(GL_ALWAYS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//transparent rendering ends
	//opaqueFB.Bind();
	opaqueScreenFB.Bind();

	// use composite shader
	compositeShader.Bind();

	// draw screen quad
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, accumScreenTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, revealScreenTex);
	quadVA.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE); //<-- enable depth writes so glClear clears the depth buffer
	glDisable(GL_BLEND);

	// bind backbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use screen shader
	screenShader.Bind();
	screenShader.SetUniform1f("gamma", 2.2);

	// draw final screen quad
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, opaqueScreenTex);
	quadVA.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void test::Test_MSTransparency::OnImGuiRender()
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

void test::Test_MSTransparency::CursorInput(double xPos, double yPos)
{
	if (mouseCtrl == 1)
		cam.ProcessMouseInput(xPos, yPos);
}

void test::Test_MSTransparency::ScrollInput(double xOffset, double yOffset)
{
	cam.ProcessScroll(xOffset, yOffset);
}