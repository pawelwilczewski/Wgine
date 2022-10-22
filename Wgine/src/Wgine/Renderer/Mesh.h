#pragma once

#include <glm/glm.hpp>
#include <Wgine/Core/Core.h>
#include "Wgine/Renderer/Vertex.h"

namespace Wgine
{
	// TODO: for performance: mark as dirty if something changed? only if dirty update ibo and vbo
	class Mesh
	{
	public:
		Mesh()
			: m_Vertices(), m_Indices()
		{}

		Mesh(Vertex *triVertices, uint32_t verticesCount, uint32_t *indices, uint32_t indicesCount)
			: m_Vertices(triVertices, triVertices + verticesCount), m_Indices(indices, indices + indicesCount)
		{
			RecalculateNormals();
		}

		const std::vector<Vertex> &GetVertices() const { return m_Vertices; }
		const std::vector<uint32_t> &GetIndices() const { return m_Indices; }

		void AddVertex(const Vertex &v) { m_Vertices.push_back(v); }
		void AddVertices(std::vector<Vertex> v) { m_Vertices.insert(m_Vertices.end(), v.begin(), v.end()); }
		void AddVertices(Vertex *v, int count) { m_Vertices.insert(m_Vertices.end(), v, v + count); }
		void AddIndex(uint32_t i) { m_Indices.push_back(i); }
		void AddIndices(uint32_t *i, int count) { m_Indices.insert(m_Indices.end(), i, i + count); }
		void AddIndices(std::vector<uint32_t> i) { m_Indices.insert(m_Indices.end(), i.begin(), i.end()); }
		void AddTriangle(const Vertex &v0, const Vertex &v1, const Vertex &v2)
		{
			uint32_t nextIndex = m_Vertices.size();
			m_Vertices.insert(m_Vertices.end(), { v0, v1, v2 });
			m_Indices.insert(m_Indices.end(), { nextIndex, nextIndex + 1, nextIndex + 2 });
		}
		void AddQuad(const Vertex &v0, const Vertex &v1, Vertex v2, Vertex v3)
		{
			uint32_t nextIndex = m_Vertices.size();
			m_Vertices.insert(m_Vertices.end(), { v0, v1, v2, v3 });
			m_Indices.insert(m_Indices.end(), { nextIndex, nextIndex + 1, nextIndex + 2, nextIndex + 2, nextIndex + 3, nextIndex });
		}

		// must share vertices
		void RecalculateNormals();

		// must not share vertices
		void RecalculateNormalsFlat();

	private:
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};
}
