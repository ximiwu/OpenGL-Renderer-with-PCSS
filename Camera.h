#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);
	
	void KeyControl(bool* keys, GLfloat deltaTime);
	void MouseControl(GLfloat xChange, GLfloat yChange);

	glm::mat4 CalculateViewMatrix();

	glm::vec3 GetCameraLocation();

	~Camera();

	bool light_moving = true;
	bool cursor_disable = true;

private:
	
	bool p_pressed = false;
	bool grave_pressed = false;

	glm::vec3 position;
	glm::vec3 front, up, right;
	glm::vec3 worldUp;

	GLfloat yaw, pitch;

	GLfloat movementSpeed;
	GLfloat turnSpeed;

	void update();

};

