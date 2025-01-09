#pragma once
#include "Renderer/Mesh.h"

class OpenGLMesh : public Mesh {
public:
    OpenGLMesh(const std::vector<Vertex>& vertices,
        const std::vector<uint32_t>& indices);

    virtual ~OpenGLMesh() = default;

    virtual void Bind() const override;
    virtual void Unbind() const override;
    virtual void Draw() const override;

    virtual const std::shared_ptr<VertexArray>& GetVertexArray() const override { return m_VertexArray; }
    virtual uint32_t GetIndexCount() const override { return m_IndexCount; }

private:
    std::shared_ptr<VertexArray> m_VertexArray;
    uint32_t m_IndexCount;
};