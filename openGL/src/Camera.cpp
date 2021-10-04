#include "Camera.h"

void Camera::UpdateCameraVector()
{
	glm::vec3 direction;
	direction.x = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
	direction.y = glm::sin(glm::radians(m_Pitch));
	direction.z = glm::cos(glm::radians(m_Pitch)) * glm::sin(glm::radians(m_Yaw));
	m_Front = glm::normalize(direction);

	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
	: m_MouseSens(d_MouseSens), m_ZoomSens(d_ZoomSens), m_MoveSpeed(d_MoveSpeed), m_Fov(d_Fov)
{
	m_Position = position;
	m_WorldUp = up;
	m_Yaw = yaw;
	m_Pitch = pitch;
	UpdateCameraVector();
}

void Camera::ProcessMovement(m_MovementStates dir, float deltaTime)
{
	float speed = m_MoveSpeed * deltaTime;
	switch (dir)
	{
	case FORWARD:	m_Position += m_Front * speed;
					break;
	case BACKWARD:	m_Position -= m_Front * speed;
					break;
	case LEFT:		m_Position -= m_Right * speed;
					break;
	case RIGHT:		m_Position += m_Right * speed;
					break;
	}
}

void Camera::ProcessMouseInput(double& xPos, double& yPos)
{
	if (m_MouseFirst)
	{
		m_lastX = (float)xPos;
		m_lastY = (float)yPos;
		m_MouseFirst = false;
	}

	float xOffset = (float)xPos - m_lastX;
	float yOffset = m_lastY - (float)yPos;
	m_lastX = (float)xPos;
	m_lastY = (float)yPos;

	xOffset *= d_MouseSens;
	yOffset *= d_MouseSens;

	m_Yaw += xOffset;
	m_Pitch += yOffset;

	if (m_Pitch > 89.0f) m_Pitch = 89.0f;
	if (m_Pitch < -89.0f) m_Pitch = -89.0f;

	UpdateCameraVector();
}

void Camera::ProcessScroll(double& xOffset, double& yOffset)
{
	m_Fov -= (float)yOffset * m_ZoomSens;
	if (m_Fov < 1.0f) m_Fov = 1.0f;
	if (m_Fov > 45.0f) m_Fov = 45.0f;
}
