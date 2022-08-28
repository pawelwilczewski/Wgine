#include "WginePCH.h"
#include "Camera.h"

#include <glm/gtx/euler_angles.hpp>

namespace Wgine
{
	void Camera::UpdateEntityMatrix()
	{
		SceneEntity::UpdateEntityMatrix();

		// TODO: this surely isn't optimized (can we just use EntityMatrix, rotate it and inverse it?)
		auto m = glm::translate(glm::mat4(1.f), m_Transform.Location);
		m = glm::rotate(m, glm::radians(m_Transform.Rotation[2]), Transform::VectorUp); // yaw rotation
		m = glm::rotate(m, glm::radians(m_Transform.Rotation[1]), Transform::VectorRight); // pitch rotation
		m = glm::rotate(m, glm::radians(m_Transform.Rotation[0]), Transform::VectorForward); // roll rotation
		//auto rot = glm::eulerAngleXYZ(glm::radians(m_Transform.Rotation[0]), glm::radians(m_Transform.Rotation[1]), glm::radians(m_Transform.Rotation[2]));
		//m *= rot;
		m = glm::scale(m, m_Transform.Scale);
		

		m_ViewMatrix = glm::inverse(m);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}
