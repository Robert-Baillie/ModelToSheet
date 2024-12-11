#include "pch.h"
#include "OpenGLTexture.h"

#include <iostream>



// Static function to convert  data types. Should be moved out
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

static GLuint CheckError(const char* context)
{
	GLuint err = glGetError();
	if (err > 0) {
		std::cout << "0x" << std::hex << err << " glGetError() in " << context
			<< std::endl;

	}
	return err;
};



OpenGLTexture::OpenGLTexture(uint32_t width, uint32_t height, const Specification& spec, unsigned char* data)
	: m_Specification(spec), m_Height(height), m_Width(width)
{
	// Generate Texture Id
	glGenTextures(1, &m_TextureID);

	// Bind and assign data
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, TextureFormatTypeToOpenGLType(m_Specification.internalFormat)
			, m_Width, m_Height, 0, TextureFormatTypeToOpenGLType(m_Specification.imageFormat)
			,GL_UNSIGNED_BYTE, data);
	CheckError("glBindTexture");


	// Set texture parameters based on specification given
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		TextureWrapTypeToOpenGLType(m_Specification.wrapS));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		TextureWrapTypeToOpenGLType(m_Specification.wrapT));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		TextureFilterTypeToOpenGLType(m_Specification.minFilter));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		TextureFilterTypeToOpenGLType(m_Specification.maxFilter));

	// Unbind
	glBindTexture(GL_TEXTURE_2D, 0);
}

OpenGLTexture::~OpenGLTexture()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	// glDeleteTextures(1, &m_TextureID);
	m_TextureID = 0;
}

void OpenGLTexture::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
}
