#include "Shader.h"

Shader::Shader(const char * vertexPath, const char * fragmentPath, const char* geometryPath)
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

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

unsigned int Shader::CompileShader(const char * shaderCode, unsigned int shaderType)
{
	unsigned int shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderCode, NULL);
	glCompileShader(shader);
	CheckCompileErrors(shader, "shader");
	return shader;
}

void Shader::Bind()
{
	glUseProgram(m_RendererID);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
}

void Shader::SetUniform1f(const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), value);
}

void Shader::SetUniform3fv(const std::string& name, glm::vec3 value)
{
	glUniform3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetUniform4fv(const std::string& name, glm::vec4 value)
{
	glUniform4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetUniform1fv(const std::string& name, glm::vec3 value)
{
	glUniform1fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetUniformMatrix4fv(const std::string& name, glm::mat4 value) {
	glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

const void Shader::CheckCompileErrors(unsigned int Shader, const std::string& type)
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
