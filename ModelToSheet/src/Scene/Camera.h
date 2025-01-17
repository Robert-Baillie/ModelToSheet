#pragma once
#include <glm/glm.hpp>


class Camera
{
public:
	const glm::vec3& GetPosition() const { return m_Position; }
	void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

	float GetRotation() const { return m_Rotation; }
	void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }


	const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
	const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
	const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

protected:
	virtual void RecalculateViewMatrix() = 0; // To be calculated per Camera Type
protected:
	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_ViewMatrix;		
	glm::mat4 m_ViewProjectionMatrix;

	glm::vec3 m_Position = { 0.0f,0.0f,0.0f };
	float m_Rotation = 0.0f;
};


class OrthographicCamera : public Camera
{
public:
	OrthographicCamera(float left, float right, float bottom, float top);
protected:
	virtual void RecalculateViewMatrix() override;
};


class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip);

	void SetProjection(float fov, float aspectRatio, float nearClip, float farClip);
	void SetAspectRatio(float ratio) {
		m_AspectRatio = ratio;
		
		RecalculateProjectionMatrix();
	}

protected:
	virtual void RecalculateViewMatrix() override;

private:
	void RecalculateProjectionMatrix();

	float m_FOV;
	float m_AspectRatio;
	float m_NearClip;
	float m_FarClip;
};