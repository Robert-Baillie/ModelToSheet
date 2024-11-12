#pragma once

#include "Renderer/VertexArray.h"
#include "Input.h"
#include "Renderer/Renderer.h"	
#include "Renderer/Camera.h"

class ExampleLayer : public Layer
{

public:
	ExampleLayer() : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) // 16:9 
	{
		// Create the vertex array (reset  destroys and creeates (essentially))
		m_VertexArray.reset(VertexArray::Create());

		// 3D coordinate in space - counter clockwise
		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.80f, 1.0f,
			 0.5f, -0.5f, 0.0f,  0.2f, 0.3f, 0.80f, 1.0f,
			 0.0f, 0.5f, 0.0f, 0.8f, 0.7f, 0.2f, 1.0f
		};
		std::shared_ptr<VertexBuffer> vertexBuffer;

		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position"},
			{ ShaderDataType::Float4, "a_Colour"}
		};
		vertexBuffer->SetLayout(layout);

		m_VertexArray->AddVertexBuffer(vertexBuffer);


		// Order of filling
		std::shared_ptr<IndexBuffer> indexBuffer;
		unsigned int indices[3] = { 0, 1, 2 };
		indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);



		// Square VA
		float sqaureVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f, 0.75f, 0.0f,
			-0.75f, 0.75f, 0.0f
		};

		m_SquareVA.reset(VertexArray::Create());
		std::shared_ptr<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(sqaureVertices, sizeof(sqaureVertices)));

		squareVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position"}
			});

		m_SquareVA->AddVertexBuffer(squareVB);

		unsigned int squareIndicies[6] = { 0, 1, 2,
										   2, 3, 0
		};
		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(squareIndicies, sizeof(squareIndicies) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		// Shader code R() makes us have multiple line string, no need for \n
		std::string vertexSrc = R"(
		#version 330 core
		
		layout(location = 0) in vec3 a_Position;
		layout(location = 1) in vec4 a_Colour;
		
		uniform mat4 u_ViewProjection;

		out vec3 v_Position;
		out vec4 v_Colour;

		void main()
		{
			v_Position = a_Position;
			v_Colour = a_Colour;
			gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
		}
		)";

		std::string fragmentSrc = R"(
		#version 330 core
		
		layout(location = 0) out vec4 color;
			
		in vec3 v_Position;
		in vec4 v_Colour;
		
		void main()
		{
			color = vec4(v_Position * 0.5 + 0.5, 1.0);
			color = v_Colour;
		}
		)";


		// Shader code R() makes us have multiple line string, no need for \n
		std::string blueShaderVertexSrc = R"(
		#version 330 core
		
		layout(location = 0) in vec3 a_Position;
		
		uniform mat4 u_ViewProjection;

	
		out vec3 v_Position;
	
		void main()
		{
			v_Position = a_Position;
			gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
		}
		)";

		std::string blueShaderFragmentSrc = R"(
		#version 330 core
		
		layout(location = 0) out vec4 color;
			
		in vec3 v_Position;
		
		void main()
		{
			color = vec4(0.2, 0.3, 0.8, 1.0);
		}
		)";

		// Create the shader - reset.
		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));
		m_BlueShader.reset(new Shader(blueShaderVertexSrc, blueShaderFragmentSrc));

	}

	void OnUpdate() override {
		if (Input::IsKeyPressed(KEY_LEFT))
			m_CameraPosition.x -= m_CameraSpeed;
		else if (Input::IsKeyPressed(KEY_RIGHT))
			m_CameraPosition.x += m_CameraSpeed;

		if (Input::IsKeyPressed(KEY_UP))
			m_CameraPosition.y += m_CameraSpeed;
		else if (Input::IsKeyPressed(KEY_DOWN))
			m_CameraPosition.y -= m_CameraSpeed;

		if (Input::IsKeyPressed(KEY_A))
			m_CameraRotation += m_CameraRotationSpeed;
		else if (Input::IsKeyPressed(KEY_D))
			m_CameraRotation -= m_CameraRotationSpeed;


		RenderCommand::SetClearColour({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();
		
		TRACE_LOG("Camera Position: {0}, {1}, {2}", m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z);
		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Renderer::BeginScene(m_Camera); // Camera Lights, Environment - all info needs to be passed.
		Renderer::Submit(m_BlueShader, m_SquareVA);		// Submit the geometry, mesh, raw vertex etc...
		Renderer::Submit(m_Shader, m_VertexArray);		// Submit the geometry, mesh, raw vertex etc...


		Renderer::EndScene(); // Finished processing info


		// Renderer::Flush(); // Usually done on a seperate thread.

	}

	void OnEvent(Event& event) override
	{
	}



private:
	// Test: Create the IDs for each
	//unsigned int m_VertexArray;
	std::shared_ptr<Shader> m_Shader;
	std::shared_ptr<VertexArray> m_VertexArray;

	std::shared_ptr<Shader> m_BlueShader;
	std::shared_ptr<VertexArray> m_SquareVA;

	// Camera on stack, when initialised
	OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
	float m_CameraSpeed = 0.1f;
	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 2;

};