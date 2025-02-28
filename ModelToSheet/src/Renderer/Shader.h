#pragma once
#include <string>

#include <glm/glm.hpp>



enum class FragmentShaderType {
	Diffuse,
	Normal,
	Specular,
	All
};


class Shader 
{
public:
	Shader(const std::string& vertexSrc, const std::string& fragmentSrc, const std::string shaderName);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void UploadUniformFloat(const std::string& name, float value);
	void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	void UploadUniformFloat3(const std::string& name, const glm::vec3& vec);
	void UploadUniformMat4Array(const std::string& name, const std::vector<glm::mat4>& matrices);

	uint32_t GetUniformLocation(const std::string& name) const;

	// Static loader
	static std::shared_ptr<Shader> CreateFromFile(const std::string& vertexPath, const std::string& fragmentPath, std::string shaderName);
	static std::string GetShaderTypeName(FragmentShaderType type)
	{
		switch (type)
		{
		case FragmentShaderType::Normal: return "normal";
		case FragmentShaderType::Specular: return "specular";
		case FragmentShaderType::Diffuse: return "diffuse";
		default: return "unknown";
		}
	}

private:
	uint32_t m_RendererID;
	std::string m_ShaderName;
};