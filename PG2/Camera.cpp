#include <glm/ext/matrix_transform.hpp>

#include "Camera.hpp"

Camera::Camera(glm::vec3 position) : Position(position)
{
    this->WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    // initialization of the camera reference system
    this->UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
}

glm::vec3 Camera::ProcessInput(GLFWwindow* window, GLfloat deltaTime)
{
    glm::vec3 direction{ 0 };

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        direction += Front;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        direction += -Front;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        direction += -Right;       // add unit vector to final direction  

    //... right, up, down, diagonal, ... 

    return glm::normalize(direction) * MovementSpeed * deltaTime;
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch = GL_TRUE)
{
    xoffset *= this->MouseSensitivity;
    yoffset *= this->MouseSensitivity;

    this->Yaw += xoffset;
    this->Pitch += yoffset;

    if (constraintPitch) {
        if (this->Pitch > 89.0f) this->Pitch = 89.0f;
        if (this->Pitch < -89.0f) this->Pitch = -89.0f;
    }

    this->UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
    front.y = sin(glm::radians(this->Pitch));
    front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));

    this->Front = glm::normalize(front);
    this->Right = glm::normalize(glm::cross(this->Front, glm::vec3(0.0f, 1.0f, 0.0f)));
    this->Up = glm::normalize(glm::cross(this->Right, this->Front));
}
