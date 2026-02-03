#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include <iostream>

Camera::Camera() :
    Position(glm::vec3{ 0.0f, 50.0f, 0.0f }),
    Yaw(-90.0f),
    Pitch(0.0f),
    Speed(10.0f),
    Sensitivity(0.1f),
    Fov(40.0f),
    Front(glm::vec3{ 0.0f, 0.0f, -1.0f }),
    Up(glm::vec3{ 0.0f, 1.0f,  0.0f }),
    Right(glm::vec3{ 1.0f, 0.0f, 0.0f }),
    lastX (0.0f), lastY(0.0f),
    firstMouse (true)
{}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(
        Position,
        Position + Front,
        //glm::vec3(0.0f, 0.0f, 0.0f),
        Up
    );
}

void Camera::ProcessKeyboard(GLFWwindow* window, float deltaTime)
{
    float velocity = Speed * deltaTime;

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W))
        Position += Front * velocity;

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S))
        Position -= Front * velocity;

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A))
        Position -= Right * velocity;

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D))
        Position += Right * velocity;

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_SPACE))
        Position += Up * velocity;

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
        Position -= Up * velocity;

    float radius = 20.0f;
	float dist = glm::length(Position);
    if (dist < radius)
    {
        Position = glm::normalize(Position) * radius;
    }
}

void Camera::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	if (cam) {
		cam->ProcessMouse(xpos, ypos);
	}
}

void Camera::ProcessMouse(float xpos, float ypos)
{
	//printf("%f\t%f\n", xpos, ypos);

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed

	lastX = xpos;
	lastY = ypos;

	/*printf("%f\t%f\n", xoffset, yoffset);*/

    xoffset *= Sensitivity;
    yoffset *= Sensitivity;

    /*printf("%f\t%f\n", xpos, ypos);*/

    Yaw += xoffset;
    Pitch += yoffset;

    Pitch = glm::clamp(static_cast<float>(Pitch), -89.0f, 89.0f);

    //std::cout << "Position: " << Position.x << " " << Position.y << " " << Position.z << '\n';
    //std::cout << "Front: " << Front.x << " " << Front.y << " " << Front.z << '\n';
    //std::cout << "Up: " << Up.x << " " << Up.y << " " << Up.z << '\n';
    //std::cout << "Right: " << Right.x << " " << Right.y << " " << Right.z << '\n';

    //std::cout << "Sense: " << Sensitivity << '\n';
    //printf("%f\n", Sensitivity);

    //std::cout << "Pitch: " << Pitch << '\n';
    //std::cout << "Yaw: " << Yaw << '\n';

    UpdateCameraVectors();
}


void Camera::UpdateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, { 0.0f, 1.0f, 0.0f }));
    Up = glm::normalize(glm::cross(Right, Front));

    Pitch = glm::clamp(static_cast<float>(Pitch), -89.0f, 89.0f);
}

void Camera::passCameraValue(GLfloat aspect, GLuint* shader_program)
{
    glm::mat4 projMatrix = glm::perspective(
        static_cast<float>(glm::radians(Fov)),
        aspect,
        0.001f,
        10000.0f
    );
    glm::mat4 cameraMatrix = GetViewMatrix();

    GLuint spm = glGetUniformLocation(*shader_program, "project");
    glUniformMatrix4fv(spm, 1, GL_FALSE, &projMatrix[0][0]);

    GLuint svm = glGetUniformLocation(*shader_program, "view");
    glUniformMatrix4fv(svm, 1, GL_FALSE, &cameraMatrix[0][0]);

    GLuint scp = glGetUniformLocation(*shader_program, "campos");
    glUniform3fv(scp, 1, glm::value_ptr(Position));
}