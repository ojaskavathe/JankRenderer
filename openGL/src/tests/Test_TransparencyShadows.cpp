#include "Test_TransparencyShadows.h"
#include "VertexBufferLayout.h"
#include "imgui/imgui.h"

test::Test_TransparencyShadows::Test_TransparencyShadows()
	:shader("res/shaders/depthMap/Lshaderv.vert", "res/shaders/depthMap/Lshaderf.frag"),
	lightShader("res/shaders/lightShaderv.vert", "res/shaders/lightShaderf.frag"),
	depthMapShader("res/shaders/depthMap/depthMapv.vert", "res/shaders/depthMap/depthMapf.frag"),
	omniDepthShader("res/shaders/omniDepthShader/oDepthMapv.vert", "res/shaders/omniDepthShader/oDepthMapf.frag", "res/shaders/omniDepthShader/oDepthMapg.geom"),
	experimental("res/shaders/shaderv.vert", "res/shaders/experimentalf.frag"),
	compositeShader("res/shaders/compositeShaderv.vert", "res/shaders/compositeShaderf.frag"),
	screenShader("res/shaders/screenShaderv.vert", "res/shaders/screenShaderf.frag")
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
	planeVA.Bind();
	VertexBuffer planeVB(planeVerts, (unsigned int)sizeof(planeVerts));
	VertexBufferLayout planeLayout;
	planeLayout.Push<float>(3);
	planeLayout.Push<float>(3);
	planeLayout.Push<float>(2);
	planeVA.AddBuffer(planeVB, planeLayout);
	planeVA.Unbind();

	//setting up framebuffers for transparency
	opaqueFB.GenTextureBufferMS(opaqueBuffer, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, GL_COLOR_ATTACHMENT0, 4);
	opaqueFB.GenTextureBufferMS(depthBuffer, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT, 4);

	//transparentFB has 3 attachments: 2 draw buffers for color, 1 depth buffer for... well, depth lmao
	transparentFB.GenTextureBufferMS(accumTexture, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, GL_COLOR_ATTACHMENT0, 4);
	transparentFB.GenTextureBufferMS(revealTexture, GL_R8, GL_RED, GL_FLOAT, GL_COLOR_ATTACHMENT1, 4);
	transparentFB.attachTextureBufferMS(GL_DEPTH_ATTACHMENT, depthBuffer); //<-the transparent framebuffer also uses the depth texture as it's order independent

	//explicitly telling opengl we have 2 draw buffers for the transparent framebuffer
	transparentFB.Bind();
	const GLenum transparentDrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, transparentDrawBuffers);
	transparentFB.Unbind();

	opaqueScreenFB.GenTextureBuffer(opaqueScreenTex, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, GL_COLOR_ATTACHMENT0);
	transparentScreenFB.GenTextureBuffer(accumScreenTex, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, GL_COLOR_ATTACHMENT0);
	transparentScreenFB.GenTextureBuffer(revealScreenTex, GL_R8, GL_RED, GL_FLOAT, GL_COLOR_ATTACHMENT1);

	//shadowmap
	depthMapFB.GenTextureBuffer(depthMap, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT, SHADOW_WIDTH, SHADOW_HEIGHT);
	depthMapFB.Bind();
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	depthMapFB.Unbind();

	//omni shadowmap
	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	//omnidirectional shadowmap framebuffer
	glGenFramebuffers(1, &oDepthMapFB);
	glBindFramebuffer(GL_FRAMEBUFFER, oDepthMapFB);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
	shader.SetUniform1i("shadowCubemap", 1);
	shader.Unbind();

	//blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//default shader uniforms
	shader.Bind();
	shader.SetUniform3fv("dirLight.color", dirLightColor);
	shader.SetUniform3fv("dirLight.ambient", dirLightAmbient);
	shader.SetUniform3fv("dirLight.diffuse", dirLightDiffuse);
	shader.SetUniform3fv("dirLight.specular", dirLightSpecular);

	shader.SetUniform3fv("pointLight.color", pointLightColor);
	shader.SetUniform3fv("pointLight.ambient", pointLightAmbient);
	shader.SetUniform3fv("pointLight.diffuse", pointLightDiffuse);
	shader.SetUniform3fv("pointLight.specular", pointLightSpecular);
	shader.SetUniform3fv("pointLight.atten", attenuationParams);

	shader.SetUniform1f("mat.shininess", matShininess);

	experimental.Bind();

	experimental.SetUniform1f("near", near);
	experimental.SetUniform1f("far", far);

	experimental.SetUniform3fv("viewPosition", cam.GetCamPosition());

	experimental.SetUniform3fv("dirLight.color", dirLightColor);
	experimental.SetUniform3fv("dirLight.ambient", dirLightAmbient);
	experimental.SetUniform3fv("dirLight.diffuse", dirLightDiffuse);
	experimental.SetUniform3fv("dirLight.specular", dirLightSpecular);

	experimental.SetUniform3fv("pointLight.color", pointLightColor);
	experimental.SetUniform3fv("pointLight.ambient", pointLightAmbient);
	experimental.SetUniform3fv("pointLight.diffuse", pointLightDiffuse);
	experimental.SetUniform3fv("pointLight.specular", pointLightSpecular);
	experimental.SetUniform3fv("pointLight.atten", attenuationParams);

	experimental.SetUniform1f("mat.shininess", matShininess);
}

test::Test_TransparencyShadows::~Test_TransparencyShadows()
{
}

void test::Test_TransparencyShadows::OnUpdate(float deltaTime, GLFWwindow* window)
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
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		pointLightPosition.y += lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		pointLightPosition.y -= lightSpeed;

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

void test::Test_TransparencyShadows::OnRender()
{
	//rendering to shadowmap
	glEnable(GL_DEPTH_TEST); //<-- keep track
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE); //<-- start writing to depth buffer
	glDisable(GL_BLEND);

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
	glEnable(GL_CULL_FACE);
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

	//render pointlight
	model = glm::mat4(1.0f);
	model = glm::translate(model, pointLightPosition);
	model = glm::scale(model, glm::vec3(0.2f));
	depthMapShader.SetUniformMatrix4fv("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//Framebuffer: Omni Shadowmap
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, oDepthMapFB); 

	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	omniDepthShader.Bind();
	omniDepthShader.SetUniform3fv("lightPos", pointLightPosition);
	omniDepthShader.SetUniform1f("far_plane", oFar);

	//set light view projection matrices
	std::vector<glm::mat4> oLightVP;
	oLightVP.push_back(shadowProj * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	oLightVP.push_back(shadowProj * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	oLightVP.push_back(shadowProj * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	oLightVP.push_back(shadowProj * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	oLightVP.push_back(shadowProj * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	oLightVP.push_back(shadowProj * glm::lookAt(pointLightPosition, pointLightPosition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
	
	for(unsigned int i = 0; i < 6; i++)
		omniDepthShader.SetUniformMatrix4fv("lightVP[" + std::to_string(i) + "]", oLightVP[i]);

	//ground
	model = glm::mat4(1.0f);
	omniDepthShader.SetUniformMatrix4fv("model", model);
	planeVA.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//cubes
	va.Bind();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	omniDepthShader.SetUniformMatrix4fv("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.5f));
	omniDepthShader.SetUniformMatrix4fv("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.25));
	omniDepthShader.SetUniformMatrix4fv("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glCullFace(GL_BACK);

	//set depth testing for opaque rendering
	glEnable(GL_DEPTH_TEST); //<-- keep track
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE); //<-- start writing to depth buffer
	glDisable(GL_BLEND);
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);

	//Framebuffer: Opaque
	opaqueFB.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE); //<- Enable face culling for opaque rendering
	glCullFace(GL_BACK);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	view = cam.GetViewMatrix(); //<-- add translation back to camera
	vp = projection * view;

	Debug::SetViewProj(vp);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

	//render pointlight
	lightShader.Bind();
	model = glm::mat4(1.0f);
	model = glm::translate(model, pointLightPosition);
	model = glm::scale(model, glm::vec3(0.2f));
	mvp = projection * view * model;
	lightShader.SetUniformMatrix4fv("mvp", mvp);
	lightShader.SetUniform3fv("lightColor", pointLightColor); // <- keep in mind MSAA won't work for values > 1
	lightVA.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//set shader uniforms
	shader.Bind();

	shader.SetUniformMatrix4fv("lightVP", lightVP);
	shader.SetUniform1f("near", near);
	shader.SetUniform1f("far", far);

	shader.SetUniform1f("oFar", oFar);

	shader.SetUniform3fv("viewPosition", cam.GetCamPosition());

	shader.SetUniform3fv("dirLight.direction", dirLightDirection);
	shader.SetUniform3fv("dirLight.color", dirLightColor);

	shader.SetUniform3fv("pointLight.position", pointLightPosition);
	shader.SetUniform3fv("pointLight.color", pointLightColor);

	shader.SetUniform1i("halfkernelWidth", halfkernelWidth);

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

	glBindFramebuffer(GL_READ_FRAMEBUFFER, opaqueFB.getID());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, opaqueScreenFB.getID());

	glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	//prepare for switching framebuffers
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunci(0, GL_ONE, GL_ONE);
	glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
	glBlendEquation(GL_FUNC_ADD);

	//Framebuffer: Transparent
	transparentFB.Bind();

	glClearBufferfv(GL_COLOR, 0, &zeroFillerVec[0]);
	glClearBufferfv(GL_COLOR, 1, &oneFillerVec[0]);

	experimental.Bind();

	experimental.SetUniform1f("near", near);
	experimental.SetUniform1f("far", far);

	experimental.SetUniform3fv("viewPosition", cam.GetCamPosition());

	experimental.SetUniform3fv("dirLight.direction", dirLightDirection);
	experimental.SetUniform3fv("dirLight.color", dirLightColor);

	experimental.SetUniform3fv("pointLight.color", pointLightColor);
	experimental.SetUniform3fv("pointLight.position", pointLightPosition);

	experimental.SetUniform1f("mat.shininess", matShininess);

	glDisable(GL_CULL_FACE); //<-disable face culling for transparent rendering

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.25));
	experimental.SetUniformMatrix4fv("model", model);
	mvp = projection * view * model;
	experimental.SetUniformMatrix4fv("mvp", mvp);
	normal = glm::transpose(glm::inverse(model));
	experimental.SetUniformMatrix4fv("normalMatrix", normal);
	experimental.SetUniform4fv("color", glm::vec4(1.0f, 0.0f, 0.0f, 0.3f));

	va.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);

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
	//Framebuffer: Opaque
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

	//Framebuffer: Backbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use screen shader
	screenShader.Bind();
	screenShader.SetUniform1f("gamma", 2.2f);
	screenShader.SetUniform1f("exposure", exposure);

	// draw final screen quad
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, opaqueScreenTex);
	quadVA.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//debug Shadowmap
	//glDisable(GL_CULL_FACE);
	//Debug::DrawOrthProj(lightVP);
	//glEnable(GL_CULL_FACE);
}

void test::Test_TransparencyShadows::OnImGuiRender()
{
	//imgui
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("color");

		ImGui::SliderFloat3("light direction", (float*)&dirLightDirection, -1.0f, 1.0f);
		ImGui::SliderFloat3("clear color", (float*)&clearColor, 0.0f, 1.0f);

		ImGui::SliderFloat3("dirLight color", (float*)&dirLightColor, 0.0f, 1.0f);
		ImGui::SliderFloat3("pointLight color", (float*)&pointLightColor, 0.0f, 1.0f);

		ImGui::SliderInt("half kernel width", &halfkernelWidth, 0, 10);
		ImGui::SliderFloat("exposure", &exposure, 0.f, 1.f);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();
	}
}

void test::Test_TransparencyShadows::CursorInput(double xPos, double yPos)
{
	if (mouseCtrl == 1)
		cam.ProcessMouseInput(xPos, yPos);
}

void test::Test_TransparencyShadows::ScrollInput(double xOffset, double yOffset)
{
	cam.ProcessScroll(xOffset, yOffset);
}