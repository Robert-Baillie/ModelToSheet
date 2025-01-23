#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "VertexArray.h"

// Create the Vertex Structure.
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

// Mesh class abstract. Fill in with platform specific renderers
class Mesh {
public:
    virtual ~Mesh() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual void Draw() const = 0;

    // Vertex Array will store info for VBO and IBO? so only this needed for the draw call.
    virtual const std::shared_ptr<VertexArray>& GetVertexArray() const = 0;
    virtual uint32_t GetIndexCount() const = 0;

    // Static function to create a mesh
    static std::shared_ptr<Mesh> Create(const std::vector<Vertex>& vertices,
        const std::vector<uint32_t>& indices);
};