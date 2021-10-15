#pragma once
#include <glad\glad.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

class Shader
{
private:
	void CheckCompileErrors(unsigned int Shader, const std::string type);

public:

	unsigned int m_RendererID;
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();

	unsigned int CompileShader(const char* shaderCode, unsigned int shaderType);

	void Bind();
	void Unbind();

	void SetUniform1i(const std::string name, int value);
	void SetUniform1f(const std::string name, float value);
	void SetUniform3fv(const std::string name, glm::vec3 value);
	void SetUniform4fv(const std::string name, glm::vec4 value);
	void SetUniform1fv(const std::string name, glm::vec3 value);
	void SetUniformMatrix4fv(const std::string name, glm::mat4 value);
};

