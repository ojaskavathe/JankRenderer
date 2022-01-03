#pragma once
#include <memory>

#include "Scenes/Scene.h"
#include "IndexBuffer.h"
#include "Camera.h"
#include "FrameBuffer.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

void SetDebugCallback();

class Renderer
{
public:
	Renderer();
	const void SetScene(Scene& scene);
	const void SetCamera(Camera& cam);
	const void LoadHDRI(const char* path);
	const void DrawScene() const;

	const void Clear() const;
	const void DrawArrays(VertexArray& va, Shader& shader) const;
	const void DrawElements(VertexArray& va, IndexBuffer& ib, Shader& shader) const;

private:
	Scene* m_Scene;
	VertexArray quadVA;
	VertexArray cubeVA;

	//Camera
	Camera* m_Camera;
	float near = 0.1f;
	float far = 100.0f;
	glm::mat4 projection;

	//Shaders
	Shader PBR = Shader("res/shaders/PBR/PBR_Modelv.vert", "res/shaders/PBR/PBR_Modelf.frag");

	Shader hdriShader = Shader("res/shaders/hdri/hdriToCubemapv.vert", "res/shaders/hdri/hdriToCubemapf.frag");
	Shader cubemapShader = Shader("res/shaders/cubemapv.vert", "res/shaders/cubemapf.frag");
	Shader irradianceShader = Shader("res/shaders/hdri/convolutionv.vert", "res/shaders/hdri/convolutionf.frag");
	Shader prefilterShader = Shader("res/shaders/hdri/prefilterv.vert", "res/shaders/hdri/prefilterf.frag");
	Shader brdfShader = Shader("res/shaders/hdri/brdfConvolutionv.vert", "res/shaders/hdri/brdfConvolutionf.frag");

	//shadows
	FrameBuffer depthMapFB;
	unsigned int depthMap;

	//IBL
	unsigned int envCubemap;
	unsigned int irradianceMap;
	unsigned int prefilterMap;
	unsigned int brdfLUT;

	unsigned int maxMipLevels = 5;

	unsigned int envFB, envRB;

	const void InitShaders() const;
};