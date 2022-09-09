#include "WginePCH.h"
#include "OpenGLVertexArray.h"

#include "glad/glad.h"

namespace Wgine
{
	static GLenum ShaderDataTypeToOpenGLBaseType(const ShaderDataType &type)
	{
		switch (type)
		{
		case ShaderDataType::Float:		return GL_FLOAT;
		case ShaderDataType::Float2:	return GL_FLOAT;
		case ShaderDataType::Float3:	return GL_FLOAT;
		case ShaderDataType::Float4:	return GL_FLOAT;
		case ShaderDataType::Mat3:		return GL_FLOAT;
		case ShaderDataType::Mat4:		return GL_FLOAT;
		case ShaderDataType::Int:		return GL_INT;
		case ShaderDataType::Int2:		return GL_INT;
		case ShaderDataType::Int3:		return GL_INT;
		case ShaderDataType::Int4:		return GL_INT;
		case ShaderDataType::Bool:		return GL_BOOL;
		}
		WGINE_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_Ptr);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_Ptr);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_Ptr);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer> &vertexBuffer)
	{
		glBindVertexArray(m_Ptr);
		vertexBuffer->Bind();

		WGINE_CORE_ASSERT(vertexBuffer->GetBufferLayout().GetElements().size(), "Vertex buffer lacks layout!");

		uint32_t index = 0;
		for (const auto &element : vertexBuffer->GetBufferLayout())
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				vertexBuffer->GetBufferLayout().GetStride(),
				(const void *)element.Offset);
			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::InsertVertexBuffer(const Ref<VertexBuffer> &vertexBuffer, uint32_t where)
	{
		glBindVertexArray(m_Ptr);
		vertexBuffer->Bind();

		WGINE_CORE_ASSERT(vertexBuffer->GetBufferLayout().GetElements().size(), "Vertex buffer lacks layout!");

		uint32_t index = 0;
		for (const auto &element : vertexBuffer->GetBufferLayout())
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				vertexBuffer->GetBufferLayout().GetStride(),
				(const void *)element.Offset);
			index++;
		}

		if (where >= m_VertexBuffers.size())
			m_VertexBuffers.resize(where + 1);
		m_VertexBuffers[where] = vertexBuffer;
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer> &indexBuffer)
	{
		glBindVertexArray(m_Ptr);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}
