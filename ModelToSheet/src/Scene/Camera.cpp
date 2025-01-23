#include "pch.h"
#include "Camera.h"



OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
{
	m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
	RecalculateViewMatrix();
}


void OrthographicCamera::RecalculateViewMatrix()
{
	// Calculate camera direction vectors
	float yawRadians = glm::radians(m_Yaw);
	float pitchRadians = glm::radians(m_Pitch);

	m_Front = glm::normalize(glm::vec3(
		sin(yawRadians) * cos(pitchRadians),
		-sin(pitchRadians),
		cos(yawRadians) * cos(pitchRadians)
	));

	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));

	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip)
	: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
{
	m_Pitch = 0.0f;   
	m_Yaw = -90.0f;    

	RecalculateProjectionMatrix();
	RecalculateViewMatrix();
}

void PerspectiveCamera::SetProjection(float fov, float aspectRatio, float nearClip, float farClip)
{
	m_FOV = fov;
	m_AspectRatio = aspectRatio;
	m_NearClip = nearClip;
	m_FarClip = farClip;
	RecalculateProjectionMatrix();
}

void PerspectiveCamera::RecalculateViewMatrix()
{

	m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);

	glm::vec3 direction;
	float pitchRad = glm::radians(m_Pitch);
	float yawRad = glm::radians(m_Yaw);

	direction.x = cos(yawRad) * cos(pitchRad);
	direction.y = sin(pitchRad);
	direction.z = sin(yawRad) * cos(pitchRad);

	m_Front = glm::normalize(direction);
	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));

	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

}

void PerspectiveCamera::RecalculateProjectionMatrix()
{
	m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

