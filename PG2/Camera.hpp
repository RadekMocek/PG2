#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera
{
public:
    // Camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up; // camera local UP vector

    GLfloat yaw = -90.0f;
    GLfloat pitch = 0.0f;
    GLfloat roll = 0.0f;

    // Camera options
    GLfloat movement_speed = 1.0f;
    GLfloat mouse_sensitivity = 0.25f;

    Camera(glm::vec3 position);
    glm::mat4 GetViewMatrix();
    glm::vec3 ProcessInput(GLFWwindow* window, GLfloat deltaTime);
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch);

private:
    glm::vec3 world_up;

    void UpdateCameraVectors();
};