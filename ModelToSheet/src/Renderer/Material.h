#pragma once
#include "Texture.h"

// Material Structures and properties
struct MaterialProperties {
	float ambient;
	float diffuse;
	float specular;
	float shininess;

	// Texture References
	Texture* diffuseMap = nullptr;
	Texture* specularMap = nullptr;
};
 
class Material {
public:

	virtual ~Material() = default;

	virtual void UseMaterial(uint32_t specularLocation, uint32_t shininessLocation) const = 0;
	virtual void SetProperties(const MaterialProperties& props) = 0;
	virtual const MaterialProperties& GetProperties() const = 0;


	// Static function to create a new material
	static Material* Create(const MaterialProperties& props);
};

