#include "pch.h"
#include "Camera.h"


void Camera::LookAt(const glm::vec3& target)
{
	// Constant World Up
	const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);

	// NCamera Vectors using this target and world up
	m_Front = glm::normalize(target - m_Position);		// Point this at the target
	m_Right = glm::normalize(glm::cross(m_Front, worldUp));		// Cros product of these
	m_Up = glm::normalize(glm::cross(m_Right, m_Front)); // Perpendiculat to the view plane

	// Calculate the pitch and yaw and recalculate
	m_Yaw = glm::degrees(atan2(m_Front.z, m_Front.x));	// Horizontal angle from +ve x axis
	m_Pitch = glm::degrees(asin(m_Front.y));	// Vertical angle from XZ plane

	RecalculateViewMatrix();
}



OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
{
	m_CameraType = Type::Orthographic;
	m_Position = glm::vec3(0.0f);
	m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
	m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_Right = glm::vec3(1.0f, 0.0f, 0.0f);

	SetProjection(left, right, bottom, top, -100.0f, 100.0f);
	RecalculateViewMatrix();
}


void OrthographicCamera::SetProjection(float left, float right, float bottom, float top, float nearClip, float farClip)
{
    m_ProjectionMatrix = glm::ortho(left, right, bottom, top, nearClip, farClip);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void OrthographicCamera::RecalculateViewMatrix()
{
	m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);

	// Calculate direction vector
	float pitchRad = glm::radians(m_Pitch);
	float yawRad = glm::radians(m_Yaw);

	glm::vec3 direction;
	direction.x = cos(yawRad) * cos(pitchRad);
	direction.y = sin(pitchRad);
	direction.z = sin(yawRad) * cos(pitchRad);

	m_Front = glm::normalize(direction);
	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));

	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip)
	: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
{
	m_CameraType = Type::Perspective;

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

	// Calculate direction vector
	float pitchRad = glm::radians(m_Pitch);
	float yawRad = glm::radians(m_Yaw);

	glm::vec3 direction;
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
	m_ViewProjectionMatrix = m_ViewProjectionMatrix * m_ViewMatrix;
}

