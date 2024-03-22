#include <iostream>

#include <glm/ext/matrix_transform.hpp>

#include "Camera.hpp"

#define print(x) std::cout << x << "\n"

Camera::Camera(glm::vec3 position) : position(position)
{
    this->world_up = glm::vec3(0.0f, 1.0f, 0.0f);
    // initialization of the camera reference system
    this->UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(this->position, this->position + this->front, this->up);
}

glm::vec3 Camera::ProcessInput(GLFWwindow* window, GLfloat delta_time)
{
    glm::vec3 direction(0,0,0);
    glm::vec3 zero(0,0,0);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        direction += front;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        direction += -front;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        direction += -right;       // add unit vector to final direction ???

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        direction += right;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        direction += up;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        direction += -up;

    float movement_speed = movement_speed_normal;

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        movement_speed = movement_speed_sprint;

    //... right, up, down, diagonal, ... 
    
    return direction == zero ? zero : glm::normalize(direction) * movement_speed * delta_time;
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{
    xoffset *= this->mouse_sensitivity;
    yoffset *= this->mouse_sensitivity;

    this->yaw += xoffset;
    //this->pitch += yoffset;
    this->pitch -= yoffset;

    // Constraint Pitch
    if (this->pitch > 89.0f) this->pitch = 89.0f;
    if (this->pitch < -89.0f) this->pitch = -89.0f;

    this->UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    front.y = sin(glm::radians(this->pitch));
    front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

    this->front = glm::normalize(front);
    this->right = glm::normalize(glm::cross(this->front, glm::vec3(0.0f, 1.0f, 0.0f)));
    this->up = glm::normalize(glm::cross(this->right, this->front));
}
