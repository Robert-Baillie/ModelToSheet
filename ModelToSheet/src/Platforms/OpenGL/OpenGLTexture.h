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

static GLenum TextureWrapTypeToOpenGLType(TextureWrap wrapType) {
	switch (wrapType)
	{
	case TextureWrap::Repeat: return GL_REPEAT;
	case TextureWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
	case TextureWrap::ClampToEdge: return GL_CLAMP_TO_EDGE;
	case TextureWrap::ClampToBorder: return GL_CLAMP_TO_BORDER;
	};

	ASSERT(false, "Unknown Wrap Type!");
	return 0;
}


static GLenum TextureFilterTypeToOpenGLType(TextureFilter filterType) {
	switch (filterType)
	{
	case TextureFilter::Nearest: return GL_NEAREST;
	case TextureFilter::Linear: return GL_LINEAR;
	case TextureFilter::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
	case TextureFilter::NearestMipmapLinear: return GL_NEAREST_MIPMAP_LINEAR;
	case TextureFilter::LinearMipmapNearest: return GL_LINEAR_MIPMAP_NEAREST;
	case TextureFilter::LinearMipmapLinear: return GL_LINEAR_MIPMAP_LINEAR;
	};

	ASSERT(false, "Unknown Texture Filter Type!");
	return 0;
}


static GLenum TextureFormatTypeToOpenGLType(TextureFormat formatType) {
	switch (formatType)
	{
	case TextureFormat::RGB: return GL_RGB;
	case TextureFormat::RGBA: return GL_RGBA;
	case TextureFormat::BGR: return GL_BGR;
	case TextureFormat::BGRA: return GL_BGRA;
	};

	ASSERT(false, "Unknown Texture Format Type!");
	return 0;
}
