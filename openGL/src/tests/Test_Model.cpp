#include <iostream>
#include "Test_Model.h"
#include "imgui/imgui.h"
#include <stb_image/stb_image.h>

test::Test_Model::Test_Model()
	:shader("res/shaders/depthMap/Lshaderv.vert", "res/shaders/depthMap/Lshaderf.frag"),
	IBLShader("res/shaders/PBR/PBR_Modelv.vert", "res/shaders/PBR/PBR_Modelf.frag"),
	lightShader("res/shaders/lightShaderv.vert", "res/shaders/lightShaderf.frag"),
	depthMapShader("res/shaders/depthMap/depthMapv.vert", "res/shaders/depthMap/depthMapf.frag"),
	omniDepthShader("res/shaders/omniDepthShader/oDepthMapv.vert", "res/shaders/omniDepthShader/oDepthMapf.frag", "res/shaders/omniDepthShader/oDepthMapg.geom"),
	experimental("res/shaders/shaderv.vert", "res/shaders/experimentalf.frag"),
	compositeShader("res/shaders/compositeShaderv.vert", "res/shaders/compositeShaderf.frag"),
	screenShader("res/shaders/screenShaderv.vert", "res/shaders/screenShaderf.frag"),
	hdriShader("res/shaders/hdri/hdriToCubemapv.vert", "res/shaders/hdri/hdriToCubemapf.frag"),
	cubemapShader("res/shaders/cubemapv.vert", "res/shaders/cubemapf.frag"),
	irradianceShader("res/shaders/hdri/convolutionv.vert", "res/shaders/hdri/convolutionf.frag"),
	prefilterShader("res/shaders/hdri/prefilterv.vert", "res/shaders/hdri/prefilterf.frag"),
	brdfShader("res/shaders/hdri/brdfConvolutionv.vert", "res/shaders/hdri/brdfConvolutionf.frag")
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
	VertexBuffer quadVB = VertexBuffer(quadVerts, (unsigned int)sizeof(quadVerts));
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

	lightPos.emplace_back(glm::vec4(1.f, 1.0f, 1.4f, 1.f));
	lightPos.emplace_back(glm::vec4(-1.f, 1.0f, 1.4f, 1.f));
	
	//SSBO
	glGenBuffers(1, &lightPosSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightPosSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, lightPos.size() * sizeof(glm::vec4), &lightPos[0], GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lightPosSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//loading hdri texture
	stbi_set_flip_vertically_on_load(true);
	int width, height, num;
	float* data = stbi_loadf("res/textures/hdri/loft.hdr", &width, &height, &num, 0);

	if (data)
	{
		glGenTextures(1, &hdrTex);
		glBindTexture(GL_TEXTURE_2D, hdrTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load HDR image. \n";
	}

	//framebuffers for environment map
	unsigned int textureRes = 1024;

	glGenFramebuffers(1, &envFB);
	glGenRenderbuffers(1, &envRB);

	glBindFramebuffer(GL_FRAMEBUFFER, envFB);

	glBindRenderbuffer(GL_RENDERBUFFER, envRB);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, textureRes, textureRes);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, envRB);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); //<-enables interpolation between faces of cubemap

	//environment cubemap texture
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, textureRes, textureRes, 0, GL_RGB, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//HDRI
	glm::mat4 hdriProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 hdriVP[] =
	{
	   hdriProj * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   hdriProj * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	   hdriProj * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	   hdriProj * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	   hdriProj * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   hdriProj * glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	hdriShader.Bind();
	hdriShader.SetUniform1i("map", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTex);

	glViewport(0, 0, textureRes, textureRes);
	glBindFramebuffer(GL_FRAMEBUFFER, envFB);

	for (int i = 0; i < 6; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		hdriShader.SetUniformMatrix4fv("vp", hdriVP[i]);
		va.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	va.Unbind();

	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	//diffuse irradiance map
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindFramebuffer(GL_FRAMEBUFFER, envFB);
	glBindRenderbuffer(GL_RENDERBUFFER, envRB);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	irradianceShader.Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	irradianceShader.SetUniform1i("environmentMap", 0);

	glViewport(0, 0, 32, 32);
	glBindFramebuffer(GL_FRAMEBUFFER, envFB);

	for (int i = 0; i < 6; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		irradianceShader.SetUniformMatrix4fv("vp", hdriVP[i]);
		va.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	va.Unbind();

	//prefilter map for specular
	glGenTextures(1, &prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	for (unsigned int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	prefilterShader.Bind();
	prefilterShader.SetUniform1i("environmentMap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	for (unsigned int mip = 0; mip < 5; ++mip)
	{
		//resize frambuffer according to mip level
		unsigned int mipWidth = 128 * glm::pow(0.5f, mip);
		unsigned int mipHeight = 128 * glm::pow(0.5f, mip);

		glBindRenderbuffer(GL_RENDERBUFFER, envRB);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float mipRoughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader.SetUniform1f("roughness", mipRoughness);

		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader.SetUniformMatrix4fv("vp", hdriVP[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			va.Bind();
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
	va.Unbind();

	glGenTextures(1, &brdfLUT);
	glBindTexture(GL_TEXTURE_2D, brdfLUT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, envFB);
	glBindRenderbuffer(GL_RENDERBUFFER, envRB);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUT, 0);

	glViewport(0, 0, 512, 512);
	brdfShader.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	quadVA.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	quadVA.Unbind();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

	//blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//default shader uniforms
	shader.Bind();
	shader.SetUniform1i("shadowMap", 0);
	shader.SetUniform1i("shadowCubemap", 1);
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

	IBLShader.Bind();
	IBLShader.SetUniform1i("hasAlbedoTex", 0);
	IBLShader.SetUniform1i("hasMetRoughTex", 0);
	IBLShader.SetUniform1i("hasNormalTex", 0);
	IBLShader.SetUniform4fv("albedoVal", glm::vec4(0.5f, 0.f, 0.f, 1.f));
	IBLShader.SetUniform3fv("pointLightColor", pointLightColor);
	IBLShader.SetUniform3fv("dirLightColor", dirLightColor);
	IBLShader.SetUniform1i("irradianceMap", 0);
	IBLShader.SetUniform1i("prefilterMap", 1);
	IBLShader.SetUniform1i("brdfLUT", 2);
	IBLShader.SetUniform1f("iblIntensity", iblIntensity);
	IBLShader.SetUniform1f("ao", 1.f);

	IBLShader.SetUniform1i("shadowMap", 3);
	IBLShader.SetUniform1i("shadowCubemap", 4);

	IBLShader.SetUniform1i("albedoTex", 5);
	IBLShader.SetUniform1i("metallicRoughnessTex", 6);
	IBLShader.SetUniform1i("normalTex", 7);
}

test::Test_Model::~Test_Model()
{
}

void test::Test_Model::OnUpdate(float deltaTime, GLFWwindow* window)
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
		lightPos[0] += glm::vec4(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cam.GetCamRight()) * lightSpeed, 1.f);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		lightPos[0] -= glm::vec4(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cam.GetCamRight()) * lightSpeed, 1.f);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		lightPos[0] -= glm::vec4(cam.GetCamRight() * lightSpeed, 1.f);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		lightPos[0] += glm::vec4(cam.GetCamRight() * lightSpeed, 1.f);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		lightPos[0].y += lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		lightPos[0].y -= lightSpeed;

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

void test::Test_Model::OnRender()
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

	mdl.DrawShadowMap(depthMapShader, lightVP);

	//ground
	model = glm::mat4(1.0f);
	depthMapShader.SetUniformMatrix4fv("model", model);
	planeVA.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Framebuffer: Omni Shadowmap
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, oDepthMapFB);

	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	omniDepthShader.Bind();
	omniDepthShader.SetUniform3fv("lightPos", lightPos[0]);
	omniDepthShader.SetUniform1f("far_plane", oFar);

	//set light view projection matrices
	std::vector<glm::mat4> oLightVP;
	oLightVP.push_back(shadowProj * glm::lookAt(glm::vec3(lightPos[0]), glm::vec3(lightPos[0]) + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	oLightVP.push_back(shadowProj * glm::lookAt(glm::vec3(lightPos[0]), glm::vec3(lightPos[0]) + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	oLightVP.push_back(shadowProj * glm::lookAt(glm::vec3(lightPos[0]), glm::vec3(lightPos[0]) + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	oLightVP.push_back(shadowProj * glm::lookAt(glm::vec3(lightPos[0]), glm::vec3(lightPos[0]) + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	oLightVP.push_back(shadowProj * glm::lookAt(glm::vec3(lightPos[0]), glm::vec3(lightPos[0]) + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	oLightVP.push_back(shadowProj * glm::lookAt(glm::vec3(lightPos[0]), glm::vec3(lightPos[0]) + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	for (unsigned int i = 0; i < 6; i++)
		omniDepthShader.SetUniformMatrix4fv("lightVP[" + std::to_string(i) + "]", oLightVP[i]);

	mdl.DrawShadowMap(omniDepthShader, vp);

	//ground
	model = glm::mat4(1.0f);
	omniDepthShader.SetUniformMatrix4fv("model", model);
	planeVA.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

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


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

	//update SSBO
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightPosSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, lightPos.size() * sizeof(glm::vec4), &lightPos[0], GL_STATIC_DRAW);

	//render pointlight
	lightShader.Bind();
	lightShader.SetUniform3fv("lightColor", pointLightColor); // <- keep in mind MSAA won't work for values > 1
	lightVA.Bind();
	for (auto& i : lightPos)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(i));
		model = glm::scale(model, glm::vec3(0.2f));
		mvp = projection * view * model;
		lightShader.SetUniformMatrix4fv("mvp", mvp);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
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


	//iblSphere
	IBLShader.Bind();
	IBLShader.SetUniform3fv("pointLightColor", pointLightColor);
	IBLShader.SetUniform3fv("camPos", cam.GetCamPosition());
	IBLShader.SetUniform3fv("dirLightDir", dirLightDirection);
	IBLShader.SetUniform4fv("albedoVal", glm::vec4(0.5f, 0.0f, 0.0f, 1.f));
	IBLShader.SetUniform1i("irradianceMap", 0);
	IBLShader.SetUniform1f("iblIntensity", iblIntensity);
	IBLShader.SetUniformMatrix4fv("lightVP", lightVP);
	IBLShader.SetUniform1f("oFar", oFar);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, brdfLUT);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

	//render ground
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.2f, 1.f, 0.2f));
	IBLShader.SetUniformMatrix4fv("model", model);
	mvp = projection * view * model;
	IBLShader.SetUniformMatrix4fv("mvp", mvp);
	normal = glm::transpose(glm::inverse(model));
	IBLShader.SetUniformMatrix4fv("normalMatrix", normal);
	IBLShader.SetUniform4fv("albedoVal", glm::vec4(0.5f));
	IBLShader.SetUniform1f("roughnessVal", 1.f);
	IBLShader.SetUniform1f("metallicVal", 0.5f);
	planeVA.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	mdl.Draw(IBLShader, vp);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//HDRI
	glDisable(GL_CULL_FACE);
	glm::mat4 cView = glm::mat4(glm::mat3(view)); // <- to get rid of translation for cubemap
	vp = projection * cView;
	glDepthFunc(GL_LEQUAL);

	cubemapShader.Bind();
	cubemapShader.SetUniformMatrix4fv("vp", vp);
	cubemapShader.SetUniform1i("box", 0);
	glActiveTexture(GL_TEXTURE0);

	if (swtch == 0)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		cubemapShader.SetUniform1f("lod", lod);
	}
	else if(swtch == 1)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	}
	else 
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		cubemapShader.SetUniform1f("lod", lod);
	}

	va.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	//regular spheres
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

	//transfer data to single sample framebuffer for postprocess
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
	screenShader.SetUniform1i("tonemap", mapped);

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

void test::Test_Model::OnImGuiRender()
{
	//imgui
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Model");

		ImGui::ColorEdit3("lightColor", &pointLightColor[0]);
		ImGui::SliderFloat("exposure", &exposure, 0.f, 1.f);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::SliderFloat("ibl Intensity", &iblIntensity, 0.f, 1.f);

		ImGui::End();
	}
}

void test::Test_Model::CursorInput(double xPos, double yPos)
{
	if (mouseCtrl == 1)
		cam.ProcessMouseInput(xPos, yPos);
}

void test::Test_Model::ScrollInput(double xOffset, double yOffset)
{
	cam.ProcessScroll(xOffset, yOffset);
}