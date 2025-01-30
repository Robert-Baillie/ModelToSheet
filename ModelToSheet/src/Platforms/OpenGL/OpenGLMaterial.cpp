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

void OpenGLMaterial::UseMaterial(FragmentShaderType shaderType, uint32_t ambientStrengthLocation, uint32_t diffuseStrengthLocation, uint32_t specularStrengthLocation, uint32_t shininessLocation) const
{
	

	// Bind Diffuse map for all
	if (m_Properties.diffuseMap) {
		// TRACE_LOG("Diffuse active");
		glActiveTexture(GL_TEXTURE0);
		m_Properties.diffuseMap->Bind();

	}

	// As above in the previous
	switch (shaderType) {
		// Normal only needs diffuse map
		case FragmentShaderType::Normal:
			break;

		// Diffuse only needs its diffuse location, and to bind the diffuse map (already done)
		case FragmentShaderType::Diffuse:
			glUniform1f(diffuseStrengthLocation, m_Properties.diffuse);
			break;

		// Specular needs specular map and the shinines and diffuse map.
		case FragmentShaderType::Specular:
			glUniform1f(specularStrengthLocation, m_Properties.specular);
			glUniform1f(shininessLocation, m_Properties.shininess);

			if (m_Properties.specularMap) {
				glActiveTexture(GL_TEXTURE1);
				// m_Properties.specularMap->Bind();
			}
			break;

		// All needs all, ofc
		case FragmentShaderType::All:
			glUniform1f(diffuseStrengthLocation, m_Properties.diffuse);
			glUniform1f(specularStrengthLocation, m_Properties.specular);
			glUniform1f(shininessLocation, m_Properties.shininess);
			glUniform1f(ambientStrengthLocation, m_Properties.ambient);
			if (m_Properties.specularMap) {
				glActiveTexture(GL_TEXTURE1);
				 m_Properties.specularMap->Bind();
			}
			break;

	}
	
}

