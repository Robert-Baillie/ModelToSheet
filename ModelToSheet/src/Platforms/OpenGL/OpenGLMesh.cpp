#include "pch.h"
#include "OpenGLMesh.h"
#include "glad/glad.h"

OpenGLMesh::OpenGLMesh(const std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices)
    : m_IndexCount(indices.size())
{
    
    m_VertexArray = VertexArray::Create();
    m_VertexArray->Bind();

    // Create vertex buffer
    std::shared_ptr<VertexBuffer> vertexBuffer = VertexBuffer::Create((float*)vertices.data(), vertices.size() * sizeof(Vertex));

    // Set the layout to match Vertex struct and shader locations
    // Create the basic layout that all meshes have
    BufferLayout layout = {
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float3, "a_Normal" },
        { ShaderDataType::Float2, "a_TexCoord" },
        { ShaderDataType::Int4,   "a_BoneIDs" },
        { ShaderDataType::Float4, "a_Weights" }
    };

  
    vertexBuffer->SetLayout(layout);
    m_VertexArray->AddVertexBuffer(vertexBuffer);

    // Create index buffer
    std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Create((uint32_t*)indices.data(), indices.size());
    if (!indexBuffer) {
        ERROR_LOG("Failed to create index buffer!");
        return;
    }
    m_VertexArray->SetIndexBuffer(indexBuffer);

    m_VertexArray->Unbind();
}

void OpenGLMesh::Bind() const
{
    m_VertexArray->Bind();
}

void OpenGLMesh::Unbind() const
{
    m_VertexArray->Unbind();
}

void OpenGLMesh::Draw() const
{
    Bind();
    glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);
    Unbind();
}