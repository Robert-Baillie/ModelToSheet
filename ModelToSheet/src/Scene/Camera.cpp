#include "pch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>



OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
{
	m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
	m_ViewMatrix = glm::mat4(1.0f);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}


void OrthographicCamera::RecalculateViewMatrix()
{
	// Transform has been changed, follow the basic rules of translating and multiplying.
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position)
		* glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));


	m_ViewMatrix = glm::inverse(transform);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip)
	: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
{
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
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position)
		* glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 1, 0));

	m_ViewMatrix = glm::inverse(transform);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void PerspectiveCamera::RecalculateProjectionMatrix()
{
	m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
}

