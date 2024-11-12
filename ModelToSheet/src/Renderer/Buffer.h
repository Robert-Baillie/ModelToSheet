#pragma once

// Shader Data Types.
enum class ShaderDataType
{
	None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
};

// Size of the Shader Data Types. Returned in bytes.
static uint32_t ShaderDataTypeSize(ShaderDataType type) {
	switch (type) {
	case ShaderDataType::Float:     return 4;
	case ShaderDataType::Float2:    return 4 * 2;
	case ShaderDataType::Float3:    return 4 * 3;
	case ShaderDataType::Float4:    return 4 * 4;
	case ShaderDataType::Mat3:      return 4 * 3 * 3;
	case ShaderDataType::Mat4:      return 4 * 4 * 4;
	case ShaderDataType::Int:       return 4;
	case ShaderDataType::Int2:      return 4 * 2;
	case ShaderDataType::Int3:      return 4 * 3;
	case ShaderDataType::Int4:      return 4 * 4;
	case ShaderDataType::Bool:      return 1;
	}

	ASSERT(false, "Unknown ShaderDataType!");
	return 0;
}

// Buffer Element Structure. Single Element within a buffer layout. 
// Abstracted aproach to buffer elements
struct BufferElement {
	std::string Name;
	ShaderDataType Type;
	uint32_t Size;
	uint32_t Offset;
	bool Normalised;

	BufferElement() {}; // Parameters left unitialised.

	// Return static function to get the size.
	// Offset is initialised at 0, this depends on the buffer element itself.
	BufferElement(ShaderDataType type, const std::string& name, bool normalised = false)
		: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalised(normalised)
	{};

	// Component Count.
	uint32_t GetComponentCount() const
	{
		switch (Type)
		{
		case ShaderDataType::Float:     return 1;
		case ShaderDataType::Float2:    return 2;
		case ShaderDataType::Float3:    return 3;
		case ShaderDataType::Float4:    return 4;
		case ShaderDataType::Mat3:      return 3 * 3;
		case ShaderDataType::Mat4:      return 4 * 4;
		case ShaderDataType::Int:       return 1;
		case ShaderDataType::Int2:      return 2;
		case ShaderDataType::Int3:      return 3;
		case ShaderDataType::Int4:      return 4;
		case ShaderDataType::Bool:      return 1;
		}

		ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}
};

class BufferLayout
{
public:
	BufferLayout() {}; // Initialised empty paramater.

	// Buffer Layout with initialiser list.
	// Allows to define a list inline with many buffer elements e.g. { {ShaderDataType::Float3, "position"}, {ShaderDataType::Float2, "texCoord"} }
	BufferLayout(const std::initializer_list<BufferElement>& elements)
		: m_Elements(elements)
	{
		CalculateOffsetsAndStride();
	};

	inline uint32_t GetStride() const { return m_Stride; }
	inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

	std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
	std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
	std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
	std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

private:
	void CalculateOffsetsAndStride()
	{
		uint32_t offset = 0; // Initialise base
		m_Stride = 0;
		for (auto& element : m_Elements)
		{
			element.Offset = offset;
			offset += element.Size; // Element offset. i.e Texture Coords, UV.
			m_Stride += element.Size; // Stride is the sum of all buffer sizes
		}
	}

private:
	std::vector<BufferElement> m_Elements;
	uint32_t m_Stride = 0;
};

/******************* Defining Buffer Types **********************/


/*********** Vertex *************/
class VertexBuffer
{
public:
	virtual ~VertexBuffer() {}

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual const BufferLayout& GetLayout() const = 0;
	virtual void SetLayout(const BufferLayout& layout) = 0;
	
	// Pass all that we want to create a vertex buffer from standard items.
	static VertexBuffer* Create(float* vertices, uint32_t size);
};


/*********** Index *************/
class  IndexBuffer
{
public:
	virtual ~IndexBuffer() {}

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;


	virtual uint32_t GetCount() const = 0;



	// Pass all that we want to create a vertex buffer from standard items.
	static IndexBuffer* Create(uint32_t* indicies, uint32_t size);
};