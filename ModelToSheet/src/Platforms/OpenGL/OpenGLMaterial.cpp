#include "pch.h"
#include "OpenGLMaterial.h"

#include "glad/glad.h"

OpenGLMaterial::OpenGLMaterial(const MaterialProperties& props)
{
	m_Properties = props;
}

OpenGLMaterial::~OpenGLMaterial()
{
	delete m_Properties.diffuseMap;
	delete m_Properties.specularMap;
}

void OpenGLMaterial::UseMaterial(uint32_t specularLocation, uint32_t shininessLocation) const
{
	glUniform1f(specularLocation, m_Properties.specular);
	glUniform1f(shininessLocation, m_Properties.shininess);
	// TRACE_LOG("Coorect Use Material being run");


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

