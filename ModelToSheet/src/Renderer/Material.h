#pragma once
#include "Texture.h"
#include "Shader.h"

// Material Structures and properties
struct MaterialProperties {
	float ambient;
	float diffuse;
	float specular;
	float shininess;

	// Texture References
	std::shared_ptr<Texture> diffuseMap = nullptr;
	std::shared_ptr<Texture> specularMap = nullptr;

	// Reference to Shader
	std::shared_ptr<Shader> shader = nullptr;
};
 
class Material {
public:

	virtual ~Material() = default;

	virtual void UseMaterial(uint32_t ambientStrengthLocation, uint32_t diffuseStrengthLocation, uint32_t specularStrengthLocation, uint32_t shininessLocation) const = 0;
	virtual void SetProperties(const MaterialProperties& props) = 0;
	virtual const MaterialProperties& GetProperties() const = 0;


	// Static function to create a new material
	static Material* Create(const MaterialProperties& props);
};

