#include "pch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>	

// Static function to convert  data types. Should this be moved out
static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
	switch (type)
	{
	case ShaderDataType::Float:     return GL_FLOAT;
	case ShaderDataType::Float2:    return GL_FLOAT;
	case ShaderDataType::Float3:    return GL_FLOAT;
	case ShaderDataType::Float4:    return GL_FLOAT;
	case ShaderDataType::Mat3:      return GL_FLOAT;
	case ShaderDataType::Mat4:      return GL_FLOAT;
	case ShaderDataType::Int:       return GL_INT;
	case ShaderDataType::Int2:      return GL_INT;
	case ShaderDataType::Int3:      return GL_INT;
	case ShaderDataType::Int4:      return GL_INT;
	case ShaderDataType::Bool:      return GL_BOOL;
	};

	ASSERT(false, "Unknown ShaderDataType!");
	return 0;
}

OpenGLVertexArray::OpenGLVertexArray()
{
	glCreateVertexArrays(1, &m_RendererID);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
	glDeleteVertexArrays(1, &m_RendererID);
}

void OpenGLVertexArray::Bind() const
{
	glBindVertexArray(m_RendererID);
}

void OpenGLVertexArray::Unbind() const
{
	glBindVertexArray(0);

}

void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
	 ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer has no layout!");

    glBindVertexArray(m_RendererID);
    vertexBuffer->Bind();

    uint32_t index = 0;
    const auto& layout = vertexBuffer->GetLayout();
    for (const auto& element : layout) {
        glEnableVertexAttribArray(index);
        
        if (element.Type == ShaderDataType::Int || 
            element.Type == ShaderDataType::Int2 || 
            element.Type == ShaderDataType::Int3 || 
            element.Type == ShaderDataType::Int4)
        {
            glVertexAttribIPointer(index,
                element.GetComponentCount(),
                ShaderDataTypeToOpenGLBaseType(element.Type),
                layout.GetStride(),
                (const void*)element.Offset);
        }
        else
        {
            glVertexAttribPointer(index,
                element.GetComponentCount(),
                ShaderDataTypeToOpenGLBaseType(element.Type),
                element.Normalised ? GL_TRUE : GL_FALSE,
                layout.GetStride(),
                (const void*)element.Offset);
        }
        index++;
    }

    m_VertexBuffers.push_back(vertexBuffer);
}

void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
	glBindVertexArray(m_RendererID);
	indexBuffer->Bind();

	m_IndexBuffer = indexBuffer;
}
