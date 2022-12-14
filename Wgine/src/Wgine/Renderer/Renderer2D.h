#pragma once

#include "Wgine/Renderer/Texture.h"
#include "Wgine/ECS/Scene.h"

#include <glm/glm.hpp>

namespace Wgine
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(Scene *scene);
		static void EndScene();

		static void Flush();

		// +x to the right (+y in 3d space)
		// +y upwards (+z in 3d space)
		static void DrawQuad(const glm::vec2 &location, float rotation, const glm::vec2 &scale, const glm::vec3 &color);

		// +x to the right (+y in 3d space)
		// +y upwards (+z in 3d space)
		static void DrawQuad(const glm::vec2 &location, float rotation, const glm::vec2 &scale, Texture2D &texture, const glm::vec2 &tiling = glm::vec2(1.f), const glm::vec4 &tint = glm::vec4(1.f));

		// in 3d space
		static void DrawQuad(const Transform &transform, const glm::vec3 &color);

		// in 3d space
		static void DrawQuad(const Transform &transform, Texture2D &texture, const glm::vec2 &tiling = glm::vec2(1.f), const glm::vec4 &tint = glm::vec4(1.f));
	};
}
