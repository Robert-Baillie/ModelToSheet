#pragma once

enum class TextureFormat {
	// Common formats
	RGB,
	RGBA,
	BGR,
	BGRA
};

enum class TextureWrap {
	Repeat,
	MirroredRepeat,
	ClampToEdge,
	ClampToBorder
};

enum class TextureFilter {
	Nearest,
	Linear,
	NearestMipmapNearest,
	NearestMipmapLinear,
	LinearMipmapNearest,
	LinearMipmapLinear
};


class Texture {
public:

	// Sruct for specification, will be converted to renderer specific datatypes.
	struct Specification {
		TextureFormat internalFormat = TextureFormat::RGBA;
		TextureFormat imageFormat = TextureFormat::RGBA;
		TextureWrap wrapS = TextureWrap::Repeat;
		TextureWrap wrapT = TextureWrap::Repeat;
		TextureFilter minFilter = TextureFilter::Linear;
		TextureFilter maxFilter = TextureFilter::Linear;
	};


	// Virtual destructure for inheritance
	virtual ~Texture() = default;

	// Virtual methods to be inplemented by the Render specific class
	virtual void Bind() const = 0;
	virtual uint32_t GetWidth() const = 0;
	virtual uint32_t GetHeight() const = 0;
	virtual const Specification& GetSpecification() const = 0;

	
	// Static function to load a texture from a file
	static std::shared_ptr<Texture> LoadFromFile(const std::string& path, const Specification& spec = Specification());

	// Helper function to get a default specification. Don't want to have to repeat this code
	static Specification GetDefaultSpecification() {
		Specification spec;
		spec.internalFormat = TextureFormat::RGBA;
		spec.imageFormat = TextureFormat::RGBA;
		spec.wrapS = TextureWrap::Repeat;
		spec.wrapT = TextureWrap::Repeat;
		spec.minFilter = TextureFilter::LinearMipmapLinear;
		spec.maxFilter = TextureFilter::Linear;
		return spec;
	}

	// Static function to create a texture
	static Texture* Create(uint32_t width, uint32_t height, const Specification& spec, unsigned char* data);
};