#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

//defaults
const float d_Yaw = -90.0f;
const float d_Pitch = 0.0f;
const float d_MouseSens = 0.07f;
const float d_ZoomSens = 1.5f;
const float d_MoveSpeed = 5.0f;
const float d_Fov = 45.0f;

class Camera
{	
private:
	void UpdateCameraVector();
	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	glm::vec3 m_WorldUp;

	float m_Fov;
	float m_Yaw;
	float m_Pitch;
	float m_MouseSens;
	float m_ZoomSens;
	float m_MoveSpeed;

	bool m_MouseFirst = true;
	float m_lastX = 400.0f, m_lastY = 300.0f;
	
public:
	enum m_MovementStates 
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = d_Yaw, float pitch = d_Pitch);
	
	void ProcessMovement(m_MovementStates dir, float deltaTime);
	void ProcessMouseInput(double& xPos, double& yPos);
	void ProcessScroll(double& xOffset, double& yOffset);

	inline glm::mat4 GetViewMatrix() { return glm::lookAt(m_Position, m_Position + m_Front, m_Up); }
	inline float GetFov() { return m_Fov; }

	inline glm::vec3 GetCamPosition() { return m_Position; }
	inline glm::vec3 GetCamFront() { return m_Front; }
	inline glm::vec3 GetCamRight() { return m_Right; }

	inline bool GetMouseFirst() { return m_MouseFirst; }
	inline void SetMouseFirst() { m_MouseFirst = true; }
};