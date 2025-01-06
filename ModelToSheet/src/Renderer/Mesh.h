#pragma once
#include "Buffer.h"
#include "VertexArray.h"
#include "Material.h"

#include <vector>

struct Vertex {
	glm::vec3 Position;
	glm::vec2 TexCoords;
	glm::vec3 Normal;

	// glm::vec3 Tangent;
	// glm::vec3 Bitangent;

	// Bone Indexes
	// int BoneIds[MAX_BONE_INFLUENCE];
	// float Weights[MAX_BONE_INFLUENCE];
};

class Mesh {
public:
	virtual ~Mesh() = default;

	// Draw this mesh with a material if supplied.
	virtual void Draw(const Material* material) const = 0;
	

	// Static for creation
	static Mesh* Create(const std::vector<Vertex>& vertices,
							const std::vector<uint32_t>& indices);

protected:
	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
};