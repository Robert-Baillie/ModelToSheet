#pragma once

#include "Renderer/VertexArray.h"
#include "Input.h"
#include "Renderer/Renderer.h"	
#include "Renderer/Camera.h"
#include "Renderer/Texture.h"

class ExampleLayer : public Layer
{
public:
    ExampleLayer() : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) // 16:9 
    {
        // Create the vertex array for a textured quad
        m_VertexArray.reset(VertexArray::Create());

        // Vertices with position and texture coordinates
        float texturedQuadVertices[5 * 4] = {
            // Position (x,y,z)   // Texture Coordinates (u,v)
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,  // Bottom Left
             0.5f, -0.5f, 0.0f,   1.0f, 0.0f,  // Bottom Right
             0.5f,  0.5f, 0.0f,   1.0f, 1.0f,  // Top Right
            -0.5f,  0.5f, 0.0f,   0.0f, 1.0f   // Top Left
        };

        std::shared_ptr<VertexBuffer> vertexBuffer;
        vertexBuffer.reset(VertexBuffer::Create(texturedQuadVertices, sizeof(texturedQuadVertices)));

        BufferLayout layout = {
            { ShaderDataType::Float3, "a_Position"},
            { ShaderDataType::Float2, "a_TexCoord"}
        };
        vertexBuffer->SetLayout(layout);

        m_VertexArray->AddVertexBuffer(vertexBuffer);

        // Index buffer for rendering the quad
        unsigned int quadIndices[6] = { 0, 1, 2, 2, 3, 0 };
        std::shared_ptr<IndexBuffer> indexBuffer;
        indexBuffer.reset(IndexBuffer::Create(quadIndices, sizeof(quadIndices) / sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(indexBuffer);

        // Texture shader with texture coordinate support
        std::string vertexSrc = R"(
        #version 330 core
        
        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec2 a_TexCoord;
        
        uniform mat4 u_ViewProjection;

        out vec2 v_TexCoord;

        void main()
        {
            v_TexCoord = a_TexCoord;
            gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
        }
        )";

        std::string fragmentSrc = R"(
        #version 330 core
        
        layout(location = 0) out vec4 color;
        
        in vec2 v_TexCoord;
        
        uniform sampler2D u_Texture;
        
        void main()
        {
            color = texture(u_Texture, v_TexCoord);
        }
        )";

        // Create texture shader
        m_TextureShader.reset(new Shader(vertexSrc, fragmentSrc));

        // Create a test texture
        Texture::Specification texSpec;
        texSpec.internalFormat = TextureFormat::RGBA;
        texSpec.imageFormat = TextureFormat::RGBA;

        // Generate a simple test texture (checkerboard pattern)
        unsigned char textureData[16 * 16 * 4];
        for (int y = 0; y < 16; y++) {
            for (int x = 0; x < 16; x++) {
                int index = (y * 16 + x) * 4;
                bool isBlack = (x + y) % 2 == 0;

                textureData[index] = isBlack ? 0 : 255;     // R
                textureData[index + 1] = isBlack ? 0 : 255;     // G
                textureData[index + 2] = isBlack ? 0 : 255;     // B
                textureData[index + 3] = 255;                   // A
            }
        }

        // Create the texture
        m_Texture.reset(Texture::Create(16, 16, texSpec, textureData));
    }

    void OnUpdate() override {
        // Camera movement controls (same as previous example)
        if (Input::IsKeyPressed(KEY_LEFT))
            m_CameraPosition.x -= m_CameraSpeed;
        else if (Input::IsKeyPressed(KEY_RIGHT))
            m_CameraPosition.x += m_CameraSpeed;

        if (Input::IsKeyPressed(KEY_UP))
            m_CameraPosition.y += m_CameraSpeed;
        else if (Input::IsKeyPressed(KEY_DOWN))
            m_CameraPosition.y -= m_CameraSpeed;

        // Clear the screen
        RenderCommand::SetClearColour({ 0.1f, 0.1f, 0.1f, 1 });
        RenderCommand::Clear();

        // Update camera
        m_Camera.SetPosition(m_CameraPosition);

        // Begin rendering
        Renderer::BeginScene(m_Camera);

        // Bind the texture before submitting
        m_Texture->Bind();

        // Submit the textured quad
        Renderer::Submit(m_TextureShader, m_VertexArray);

        // End rendering
        Renderer::EndScene();
    }

    void OnEvent(Event& event) override
    {
        // Event handling can be added here if needed
    }

private:
    std::shared_ptr<Shader> m_TextureShader;
    std::shared_ptr<VertexArray> m_VertexArray;
    std::shared_ptr<Texture> m_Texture;

    // Camera setup (same as previous example)
    OrthographicCamera m_Camera;
    glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
    float m_CameraSpeed = 0.1f;
};