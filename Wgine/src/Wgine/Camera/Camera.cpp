#include "WginePCH.h"
#include "Camera.h"
#include "Wgine/Core/Application.h"

namespace Wgine
{
	void Camera::UpdateEntityMatrix()
	{
		SceneEntity::UpdateEntityMatrix();

		// TODO: this surely isn't optimized (can we just use EntityMatrix, rotate it and inverse it?)
		auto m = glm::translate(glm::mat4(1.f), m_Transform.Location);
		m = glm::rotate(m, glm::radians(180.f), GetRightVector()); // pitch rotation
		m = glm::rotate(m, glm::radians(m_Transform.Rotation[2]), GetUpVector()); // yaw rotation
		m = glm::rotate(m, glm::radians(m_Transform.Rotation[1]), GetRightVector()); // pitch rotation
		m = glm::rotate(m, glm::radians(m_Transform.Rotation[0]), GetForwardVector()); // roll rotation
		m = glm::scale(m, m_Transform.Scale);

		m_ViewMatrix = glm::inverse(m);

		RecalculateViewProjectionMatrix();
	}

	void Camera::OnStart()
	{
		UpdateWindowSize((float)Application::Get().GetWindow().GetWidth(), (float)Application::Get().GetWindow().GetHeight());
		UpdateProjectionMatrix();
	}

	void Camera::OnEvent(Event &e)
	{
		EventDispatcher ed(e);
		ed.Dispatch<WindowResizeEvent>(WGINE_BIND_EVENT_FN(Camera::OnWindowResized));
	}

	bool Camera::OnWindowResized(WindowResizeEvent &e)
	{
		UpdateWindowSize((float)e.GetWidth(), (float)e.GetHeight());
		return false;
	}

	void PerspectiveCamera::UpdateWindowSize(float width, float height)
	{
		SetWindowSize(width, height);
	}

	void PerspectiveCamera::UpdateProjectionMatrix()
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_Width / m_Height, m_NearClip, m_FarClip);
		// we want the camera to face +x:
		m_ProjectionMatrix = glm::rotate(m_ProjectionMatrix, glm::radians(-90.f), Transform::s_VectorUp);
		m_ProjectionMatrix = glm::rotate(m_ProjectionMatrix, glm::radians(90.f), Transform::s_VectorRight);

		RecalculateViewProjectionMatrix();
	}

	void OrthographicCamera::UpdateWindowSize(float width, float height)
	{
		auto aspectRatio = width / height;
		SetBottom(-m_HalfSize);
		SetLeft(-aspectRatio * m_HalfSize);
		SetUp(m_HalfSize);
		SetRight(aspectRatio * m_HalfSize);
	}

	void OrthographicCamera::UpdateProjectionMatrix()
	{
		m_ProjectionMatrix = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, m_NearClip, m_FarClip);
		// we want the camera to face +x:
		m_ProjectionMatrix = glm::rotate(m_ProjectionMatrix, glm::radians(-90.f), Transform::s_VectorUp);
		m_ProjectionMatrix = glm::rotate(m_ProjectionMatrix, glm::radians(90.f), Transform::s_VectorRight);

		RecalculateViewProjectionMatrix();
	}
}
