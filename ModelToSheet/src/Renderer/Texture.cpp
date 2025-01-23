#include "pch.h"	
#include "Texture.h"

#include "Renderer.h"

#include "Platforms/OpenGL/OpenGLTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


std::shared_ptr<Texture> Texture::LoadFromFile(const std::string& path, const Specification& spec)
{
    // Load image data using stb_image
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);

    if (!data) {
        ERROR_LOG("Failed to load texture: {0}", path);
        return nullptr;
    }

    // Create the texture
    Texture* texture = Create(width, height, spec, data);

    // Free the image data
    stbi_image_free(data);

    return std::shared_ptr<Texture>(texture);
}

std::shared_ptr<Texture> Texture::LoadFromMemory(const unsigned char* compressedData, uint32_t dataSize, const Specification& spec)
{
    int width, height, channels;
    unsigned char* decocodedData = stbi_load_from_memory(compressedData, static_cast<int>(dataSize), &width, &height, &channels, 4); //RGBA Behaviour

    if (!decocodedData) {
        ERROR_LOG("Failed to decode texture from memory");
        return nullptr;
    }

    // Create the texture with dataa
    Texture* texture = Create(width, height, spec, decocodedData);
    stbi_image_free(decocodedData);
    return std::shared_ptr<Texture>(texture);
}


Texture* Texture::Create(uint32_t width, uint32_t height, const Specification& spec, unsigned char* data)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		ASSERT(false, "Renderer API None is not supported!");
		return nullptr;
	case RendererAPI::API::OpenGL:
		return new OpenGLTexture(width, height,spec, data);
	}

	ASSERT(false, "Unknown Renderer API!");
	return nullptr;
}