#include <glad\glad.h>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;

	//exceptions
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		if (geometryPath != nullptr)
		{
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure)
	{
		std::cout << "[Shader]: Error Reading Shader." << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int vertex = CompileShader(vShaderCode, GL_VERTEX_SHADER);
	unsigned int fragment = CompileShader(fShaderCode, GL_FRAGMENT_SHADER);

	m_RendererID = glCreateProgram();
	glAttachShader(m_RendererID, vertex);
	glAttachShader(m_RendererID, fragment);

	unsigned int geometry;
	if (geometryPath != nullptr)
	{
		const char* gShaderCode = geometryCode.c_str();
		geometry = CompileShader(gShaderCode, GL_GEOMETRY_SHADER);
		glAttachShader(m_RendererID, geometry);
	}

	glLinkProgram(m_RendererID);
	CheckCompileErrors(m_RendererID, "m_RendererID");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath != nullptr) glDeleteShader(geometry);
}

Shader::Shader(const char* computePath)
{
	std::string computeCode;
	std::ifstream cShaderFile;

	//exceptions
	cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		cShaderFile.open(computePath);

		std::stringstream cShaderStream;
		cShaderStream << cShaderFile.rdbuf();

		computeCode = cShaderStream.str();
	}
	catch (std::ifstream::failure)
	{
		std::cout << "[Compute Shader]: Error Reading Shader. \n Path: " << computePath << "\n";
	}

	const char* cShaderCode = computeCode.c_str();

	unsigned int compute = CompileShader(cShaderCode, GL_COMPUTE_SHADER);

	m_RendererID = glCreateProgram();
	glAttachShader(m_RendererID, compute);

	glLinkProgram(m_RendererID);
	CheckCompileErrors(m_RendererID, "m_RendererID");

	glDeleteShader(compute);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

const unsigned int Shader::CompileShader(const char * shaderCode, unsigned int shaderType) const
{
	unsigned int shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderCode, NULL);
	glCompileShader(shader);
	CheckCompileErrors(shader, "shader");
	return shader;
}

const void Shader::Bind() const
{
	glUseProgram(m_RendererID);
}

const void Shader::Unbind() const
{
	glUseProgram(0);
}

const void Shader::SetUniform1i(const std::string& name, const int& value) const
{
	glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
}

const void Shader::SetUniform1f(const std::string& name, const float& value) const
{
	glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), value);
}

const void Shader::SetUniform3fv(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
}

const void Shader::SetUniform4fv(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
}

const void Shader::SetUniform1fv(const std::string& name, const glm::vec3& value) const
{
	glUniform1fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
}

const void Shader::SetUniformMatrix4fv(const std::string& name, const glm::mat4& value) const 
{
	glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

const void Shader::CheckCompileErrors(unsigned int& Shader, const std::string& type) const
{
	int success;
	char infolog[1024];

	if (type != "m_RendererID")
	{
		glGetShaderiv(Shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(Shader, 1024, NULL, infolog);
			std::cout << "[Shader]: Compiler Error. \n" << infolog << "\n\n";
		}
	}
	else {
		glGetProgramiv(Shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(Shader, 1024, NULL, infolog);
			std::cout << "[Shader]: Linker Error. \n" << infolog << "\n\n";
		}
	}
}
