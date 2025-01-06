#pragma once
#include "Renderer/Mesh.h"

class OpenGLMesh : public Mesh {
public:
	OpenGLMesh(const std::vector<Vertex>& vertices,
		const std::vector<uint32_t>& indices);

	virtual ~OpenGLMesh();

	virtual void Draw(const Material* material) const override;

private:
	SetUpMesh();

private:
	// VAO, VBO and IBO needed for OpenGL Set Up.
	VertexArray* m_VertexArray;
	VertexBuffer* m_VertexBuffer;
	IndexBuffer* m_IndexBuffer;
};