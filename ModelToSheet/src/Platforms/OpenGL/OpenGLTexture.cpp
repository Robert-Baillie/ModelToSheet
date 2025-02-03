#include "pch.h"
#include "OpenGLTexture.h"




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
