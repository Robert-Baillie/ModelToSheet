#pragma once
#include "Renderer/Texture.h"

#include "glad/glad.h"
#include "glfw/glfw3.h"

class OpenGLTexture : public Texture {
public:
	OpenGLTexture(uint32_t width, uint32_t height, const Specification& spec, unsigned char* data);

	virtual ~OpenGLTexture();

	
	virtual void Bind() const override ;
	virtual uint32_t GetWidth() const override { return m_Width; }
	virtual uint32_t GetHeight()const  override { return m_Height; }
	virtual const Specification& GetSpecification() const override { return m_Specification; }

private:
	GLuint m_TextureID;
	Specification m_Specification;
	GLuint m_Width, m_Height;
};