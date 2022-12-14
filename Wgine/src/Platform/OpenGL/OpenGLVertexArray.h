#pragma once

#include "Wgine/Renderer/VertexArray.h"

namespace Wgine
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer> &vertexBuffer) override;
		virtual void SetVertexBuffer(const Ref<VertexBuffer> &vertexBuffer, uint32_t index) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer> &indexBuffer) override;

		virtual const std::vector<Ref<VertexBuffer>> GetVertexBuffers() const override { return m_VertexBuffers; };
		virtual const Ref<IndexBuffer> GetIndexBuffer() const override { return m_IndexBuffer; };

	private:
		uint32_t m_Ptr;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}
