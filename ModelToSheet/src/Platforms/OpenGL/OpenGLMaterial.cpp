#include "pch.h"
#include "OpenGLMaterial.h"

#include "glad/glad.h"

OpenGLMaterial::OpenGLMaterial(const MaterialProperties& props)
{
	m_Properties = props;
}

OpenGLMaterial::~OpenGLMaterial()
{
	m_Properties.diffuseMap = nullptr;
	m_Properties.specularMap = nullptr;
}

void OpenGLMaterial::UseMaterial(uint32_t ambientStrengthLocation, uint32_t diffuseStrengthLocation, uint32_t specularStrengthLocation, uint32_t shininessLocation) const
{
	// Bind the shader if we have set it, otherwise it will use default (or whatever is loaded)
	if (m_Properties.shader) {
		m_Properties.shader->Bind(); 
	}

	glUniform1f(ambientStrengthLocation, m_Properties.ambient);
	glUniform1f(diffuseStrengthLocation, m_Properties.diffuse);
	glUniform1f(specularStrengthLocation, m_Properties.specular);
	glUniform1f(shininessLocation, m_Properties.shininess);
	

	if (m_Properties.diffuseMap) {
		// TRACE_LOG("Diffuse active");
		glActiveTexture(GL_TEXTURE0);
		m_Properties.diffuseMap->Bind();

	}

	if (m_Properties.specularMap) {
		// TRACE_LOG("UniformLocation Spec: {0}", specularLocation);
		glActiveTexture(GL_TEXTURE1);
		m_Properties.specularMap->Bind();
	}
}

