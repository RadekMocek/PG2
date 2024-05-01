#include "App.hpp"

glm::vec3 App::BallMovement(GLfloat delta_time)
{
	glm::vec3 zero(0, 0, 0);
	glm::vec3 direction(0, 0, 0);
	glm::vec3 pseudo_front(1, 0, 0);
	glm::vec3 pseudo_right(0, 0, 1);
	glm::vec3 pseudo_up(0, 1, 0);
	if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS) {
		direction += pseudo_front;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS) {
		direction += -pseudo_front;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS) {
		direction += -pseudo_right;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS) {
		direction += pseudo_right;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS) {
		direction += pseudo_up;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS) {
		direction += -pseudo_up;
	}
	return direction == zero ? zero : glm::normalize(direction) * delta_time;
}
