#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
	bool firstMouse = true;
	double lastX, lastY;
public:
	glm::vec3 Position{ 0.0f, 50.0f, 0.0f };

	double Yaw = -90.0f; // чтобы смотреть вдоль -Z
	double Pitch = 0.0f;

	double Speed = 10.0f;
	double Sensitivity = 0.1f;
	double Fov = 40.0f;

	glm::vec3 Front{ 0.0f, 0.0f, -1.0f };
	glm::vec3 Up{ 0.0f, 1.0f,  0.0f };
	glm::vec3 Right{ 1.0f, 0.0f, 0.0f };

	Camera();

	void ProcessKeyboard(GLFWwindow* window, float deltaTime);
	void passCameraValue(GLfloat aspect, GLuint* shader_program);
	void ProcessMouse(float xOffset, float yOffset);
	void UpdateCameraVectors();
	static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
	glm::mat4 GetViewMatrix();
};
