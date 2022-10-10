#pragma once
#include <glm\glm.hpp>
#include <string>

class Shader
{
private:
	const void CheckCompileErrors(unsigned int& Shader, const std::string& type) const;

public:

	unsigned int m_RendererID;
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
	Shader(const char* computePath);
	~Shader();

	const unsigned int CompileShader(const char* shaderCode, unsigned int shaderType) const;

	const void Bind() const;
	const void Unbind() const;

	const void SetUniform1i(const std::string& name, const int& value) const;
	const void SetUniform1f(const std::string& name, const float& value) const;
	const void SetUniform3fv(const std::string& name, const glm::vec3& value) const;
	const void SetUniform4fv(const std::string& name, const glm::vec4& value) const;
	const void SetUniform1fv(const std::string& name, const glm::vec3& value) const;
	const void SetUniformMatrix4fv(const std::string& name, const glm::mat4& value) const;
};

