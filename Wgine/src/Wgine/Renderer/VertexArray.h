#pragma once

#include "Wgine/Renderer/Buffer.h"
#include <memory>

namespace Wgine
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(const Ref<VertexBuffer> &vertexBuffer) = 0;
		virtual void SetVertexBuffer(const Ref<VertexBuffer> &vertexBuffer, uint32_t index) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer> &indexBuffer) = 0;

		virtual const std::vector<Ref<VertexBuffer>> GetVertexBuffers() const = 0;
		virtual const Ref<IndexBuffer> GetIndexBuffer() const = 0;

		static Ref<VertexArray> Create();
	};
}
