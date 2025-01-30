#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	enum class Type {
		Perspective,
		Orthographic
	};

	const Type GetCameraType() { return m_CameraType; }

	const glm::vec3& GetPosition() const { return m_Position; }
	void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

	float GetPitch() const { return m_Pitch; }
	float GetYaw() const { return m_Yaw; }

	// Camera Rotation
	void SetPitch(float pitch) {
		m_Pitch = pitch;
		RecalculateViewMatrix();
	}

	void SetYaw(float yaw) {
		m_Yaw = yaw;
		RecalculateViewMatrix();
	}

	// Camera Movement
	void MoveForward(float deltaTime) { Move(m_Front * m_MoveSpeed * deltaTime); }
	void MoveBackward(float deltaTime) { Move(-m_Front * m_MoveSpeed * deltaTime); }
	void MoveRight(float deltaTime) { Move(m_Right * m_MoveSpeed * deltaTime); }
	void MoveLeft(float deltaTime) { Move(-m_Right * m_MoveSpeed * deltaTime); }
	void MoveUp(float deltaTime) { Move(m_Up * m_MoveSpeed * deltaTime); }
	void MoveDown(float deltaTime) { Move(-m_Up * m_MoveSpeed * deltaTime); }

	// For calculations
	const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
	const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
	const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

protected:
	virtual void RecalculateViewMatrix() = 0; // To be calculated per Camera Type

	// Simply move by the offset given
	void Move(const glm::vec3& offset) {
		// TRACE_LOG("Attempting move: {0}, {1}, {2}", offset.x, offset.y, offset.z);
		m_Position += offset;
		RecalculateViewMatrix();
	}

protected:
	Type m_CameraType;

	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_ViewMatrix;		
	glm::mat4 m_ViewProjectionMatrix;
	

	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	glm::vec3 m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float m_Yaw =  0.0f; // Left and Right (rotate y axis)
	float m_Pitch= 0.0f; // Up and Down (roate x axis)

	float m_MoveSpeed = 5.0f;
	float m_RotationSpeed =0.1f;
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