#include <iostream>

#include "App.hpp"

void App::error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void App::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if ((action == GLFW_PRESS) || (action == GLFW_REPEAT)) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        case GLFW_KEY_F:
            is_fullscreen_on = !is_fullscreen_on;
            if (is_fullscreen_on) {
                glfwGetWindowPos(window, &window_xcor, &window_ycor);
                glfwGetWindowSize(window, &window_width_return_from_fullscreen, &window_height_return_from_fullscreen);
                if (window_height_return_from_fullscreen == 0) window_height_return_from_fullscreen++;
                glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            }
            else {
                glfwSetWindowMonitor(window, nullptr, window_xcor, window_ycor, window_width_return_from_fullscreen, window_height_return_from_fullscreen, 0);
            }
            break;
        case GLFW_KEY_V:
            is_vsync_on = !is_vsync_on;
            glfwSwapInterval(is_vsync_on);
            std::cout << "VSync: " << is_vsync_on << "\n";
            break;
        }
    }
    
    // Minecraft sprint
    if (action == GLFW_PRESS && key == GLFW_KEY_LEFT_CONTROL) {
        camera.ToggleSprint();
    }
}

void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    this_inst->FOV += 10.0f * static_cast<float>(yoffset);
    this_inst->FOV = std::clamp(this_inst->FOV, 20.0f, 170.0f); // limit FOV to reasonable values...
    this_inst->UpdateProjectionMatrix();
}

void App::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    ///*
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    this_inst->window_width = width;
    this_inst->window_height = height;
    // set viewport
    glViewport(0, 0, width, height);
    //now your canvas has [0,0] in bottom left corner, and its size is [width x height] 
    this_inst->UpdateProjectionMatrix();
    /**/

    /*
    glViewport(0, 0, width, height);
    /**/
}

void App::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        //std::cout << "Right click!\n";
    }
}

void App::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    ///*
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        camera.ProcessMouseMovement(static_cast<GLfloat>(xpos - last_cursor_xpos) , static_cast<GLfloat>(ypos - last_cursor_ypos));
    }
    last_cursor_xpos = xpos;
    last_cursor_ypos = ypos;
    /**/
}
