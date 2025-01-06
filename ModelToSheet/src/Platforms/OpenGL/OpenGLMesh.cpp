#include "pch.h"
#include "OpenGLMesh.h"

OpenGLMesh::OpenGLMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
{
	m_Vertices = vertices;
	m_Indices = indices;

	SetUpMesh();
}

OpenGLMesh::~OpenGLMesh()
{
	delete m_VertexArray;
	delete m_VertexBuffer;
	delete m_IndexBuffer;
}


OpenGLMesh::SetUpMesh()
{
	// Create Buffers
	m_VertexArray = VertexArray::Create();
	m_VertexBuffer = VertexBuffer::Create(&m_Vertices[0], m_Vertices.size() * sizeof(Vertex));
	m_IndexBuffer = IndexBuffer::Create(&m_Indices[0], m_Indices.size());

	// Buffer Layout
	BufferLayout layout = {
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float3, "a_Normal" },
		{ ShaderDataType::Float2, "a_TexCoord" }
	};
	
	m_VertexBuffer->SetLayout(layout);

	// Add Vertex Buffer to VertexArray
	m_VertexArray->AddVertexBuffer(m_VertexBuffer);
	m_VertexArray->SetIndexBuffer(m_IndexBuffer);
}


void OpenGLMesh::Draw() const
{
	// Bind and draw the mesh
	m_VertexArray->Bind();
	glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
}


