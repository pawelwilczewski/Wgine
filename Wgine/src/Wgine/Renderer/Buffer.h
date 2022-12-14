#pragma once

namespace Wgine
{
	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:		return 4;
			case ShaderDataType::Float2:	return 4 * 2;
			case ShaderDataType::Float3:	return 4 * 3;
			case ShaderDataType::Float4:	return 4 * 4;
			case ShaderDataType::Mat3:		return 4 * 3 * 3;
			case ShaderDataType::Mat4:		return 4 * 4 * 4;
			case ShaderDataType::Int:		return 4;
			case ShaderDataType::Int2:		return 4 * 2;
			case ShaderDataType::Int3:		return 4 * 3;
			case ShaderDataType::Int4:		return 4 * 4;
			case ShaderDataType::Bool:		return 1;
		}
		WGINE_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement
	{
		std::string Name = "";
		ShaderDataType Type = ShaderDataType::None;
		uint32_t Size = ShaderDataTypeSize(ShaderDataType::None);
		uint32_t Offset = 0;
		bool Normalized = false;

		BufferElement()
		{}
		BufferElement(ShaderDataType type, const std::string &name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case ShaderDataType::Float:		return 1;
				case ShaderDataType::Float2:	return 2;
				case ShaderDataType::Float3:	return 3;
				case ShaderDataType::Float4:	return 4;
				case ShaderDataType::Mat3:		return 3 * 3;
				case ShaderDataType::Mat4:		return 4 * 4;
				case ShaderDataType::Int:		return 1;
				case ShaderDataType::Int2:		return 2;
				case ShaderDataType::Int3:		return 3;
				case ShaderDataType::Int4:		return 4;
				case ShaderDataType::Bool:		return 1;
			}
			WGINE_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}
		BufferLayout(const std::initializer_list<BufferElement> &elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		inline const std::vector<BufferElement> &GetElements() const { return m_Elements; }
		inline uint32_t GetStride() const { return m_Stride; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		inline void CalculateOffsetsAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto &element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}

	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class VertexBuffer // TODO: incorporate template specialization?
	{
	public:
		static Ref<VertexBuffer> Create(uint32_t size);
		static Ref<VertexBuffer> Create(void *vertices, uint32_t size);

	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		// will automatically resize to twice the required amount if exceeded the current size
		// will invalidate the data up to the offset if exceeded the current size
		virtual void SetData(const void *data, uint32_t size, uint32_t offsetBytes = 0) = 0;
		// doesnt keep the elements currently stored; only resized if bigger size than current
		virtual void Resize(uint32_t newSize) = 0;
		virtual void SetLayout(const BufferLayout &layout) = 0;

		virtual uint32_t GetSize() const = 0;
		virtual const BufferLayout &GetBufferLayout() const = 0;
	};

	class IndexBuffer // TODO: incorporate template specialization?
	{
	public:
		static Ref<IndexBuffer> Create(uint32_t count);
		static Ref<IndexBuffer> Create(uint32_t *indices, uint32_t count);

	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		// will automatically resize to twice the required amount if exceeded the current size
		// will invalidate the data up to the offset if exceeded the current size
		virtual void SetData(const void *data, uint32_t count, uint32_t offsetCount = 0) = 0;
		// doesnt keep the elements currently stored; only resized if bigger size than current
		virtual void Resize(uint32_t newCount) = 0;

		virtual uint32_t GetCount() const = 0;
	};

	class StorageBuffer // TODO: incorporate template specialization?
	{
	public:
		virtual ~StorageBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		// will automatically resize to twice the required amount if exceeded the current size
		// will invalidate the data up to the offset if exceeded the current size
		virtual void SetData(const void *data, uint32_t size, uint32_t offsetBytes = 0) = 0;
		// doesnt keep the elements currently stored; only resized if bigger size than current
		virtual void Resize(uint32_t newSize) = 0;

		virtual uint32_t GetSize() const = 0;
		virtual uint32_t GetPtr() const = 0;

		static Ref<StorageBuffer> Create(uint32_t size);
		static Ref<StorageBuffer> Create(const void *data, uint32_t size);
	};
}
