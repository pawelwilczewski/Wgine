#pragma once

#include "Wgine/Core/Transform.h"
#include "Wgine/Renderer/VertexArray.h"
#include "Wgine/Renderer/Shader.h"
#include "Wgine/Core/Core.h"
#include "Wgine/Event/Event.h"

namespace Wgine
{
	class Entity
	{
		friend class Scene;
	public:
		Entity() {}

	protected:
		virtual void OnStart() {}
		virtual void OnTick(float deltaSeconds) {}
		virtual void OnEnd() {}
		virtual void OnEvent(Event &e) {}

	public:
		class Scene *GetScene() const { return m_Scene; }

		// TODO: this (and the system dedicated to checking this) may be very costly for large scenes?
		bool GetPropagatesEvents() const { return m_PropagatesEvents; }
		void SetPropagatesEvents(const bool &propagates) { m_PropagatesEvents = propagates; }

	private:
		bool m_PropagatesEvents = false;
		Scene *m_Scene = nullptr; // TODO: this is not memory efficient?
	};

	class SceneEntity : public Entity
	{
	public:
		SceneEntity()
			: Entity(), m_Transform(Transform())
		{
			UpdateEntityMatrix();
		}

		SceneEntity(Transform transform)
			: Entity(), m_Transform(transform)
		{
			UpdateEntityMatrix();
		}

	public:
		// TODO: in the future this should be a part of transform component so this class is simplified and the actual ECS will improve performance
		const Transform &GetTransform() const { return m_Transform; }
		const glm::mat4 &GetEntityMatrix() const { return m_EntityMatrix; }
		const glm::vec3 &GetLocation() const { return m_Transform.Location; }
		const glm::vec3 &GetRotation() const { return m_Transform.Rotation; }
		const glm::vec3 &GetScale() const { return m_Transform.Scale; }
		const glm::vec3 &GetForwardVector() const { return m_Transform.GetForwardVector(); }
		const glm::vec3 &GetRightVector() const { return m_Transform.GetRightVector(); }
		const glm::vec3 &GetUpVector() const { return m_Transform.GetUpVector(); }

		void SetTransform(Transform t) { m_Transform = t; UpdateEntityMatrix(); }
		void SetLocation(glm::vec3 location) { m_Transform.Location = location; UpdateEntityMatrix(); }
		void SetRotation(glm::vec3 rotation) { m_Transform.Rotation = rotation; UpdateEntityMatrix(); }
		void SetScale(glm::vec3 scale) { m_Transform.Scale = scale; UpdateEntityMatrix(); }

		// TEMP
		Ref<VertexArray> MeshData;
		Ref<Shader> ShaderData;

	protected:
		virtual void UpdateEntityMatrix() { m_EntityMatrix = m_Transform.ToModelMatrix(); }

	protected:
		Transform m_Transform;
		glm::mat4 m_EntityMatrix;
	};
}