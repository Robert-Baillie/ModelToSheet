#pragma once
#include "Renderer/Material.h"

class OpenGLMaterial : public Material {
public:
	OpenGLMaterial(const MaterialProperties& props);

	virtual ~OpenGLMaterial();

	
	virtual void UseMaterial(uint32_t ambientStrengthLocation, uint32_t diffuseStrengthLocation, uint32_t specularStrengthLocation, uint32_t shininessLocation) const override;
	virtual void SetProperties(const MaterialProperties& props)  override { m_Properties = props; }
	virtual const MaterialProperties& GetProperties() const override { return m_Properties; }



private:
	MaterialProperties m_Properties;
};