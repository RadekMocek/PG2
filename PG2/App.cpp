// Basic includes
#include <iostream>
#include <chrono>
#include <stack>
#include <random>
#include <sstream>

// OpenCV – GL independent
//#include <opencv2\opencv.hpp>

// OpenGL Extension Wrangler: allow all multiplatform GL functions
#include <GL/glew.h> 
// WGLEW = Windows GL Extension Wrangler (change for different platform) 
// platform specific functions (in this case Windows)
#include <GL/wglew.h> 

// GLFW toolkit
// Uses GL calls to open GL context, i.e. GLEW must be first.
#include <GLFW/glfw3.h>

// OpenGL math
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Our app
#include "App.hpp"
#include "gl_err_callback.hpp"

bool App::is_vsync_on = false;
bool App::is_fullscreen_on = false;
GLFWmonitor* App::monitor;
const GLFWvidmode* App::mode;
int App::window_xcor{};
int App::window_ycor{};
int App::win_width = 800;
int App::win_height = 600;

App::App()
{
    // default constructor
    // nothing to do here (for now...)
    std::cout << "Constructed...\n";
}

// App initialization, if returns true then run run()
bool App::init()
{
    try {
        // Set GLFW error callback
        glfwSetErrorCallback(error_callback);

        // Init GLFW :: https://www.glfw.org/documentation.html
        if (!glfwInit()) {
            return false;
        }

        // Set OpenGL version
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        // Set OpenGL profile
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core, comment this line for Compatible

        // Open window (GL canvas) with no special properties :: https://www.glfw.org/docs/latest/quick.html#quick_create_window
        window = glfwCreateWindow(win_width, win_height, "Moje krasne okno", NULL, NULL);
        if (!window) {
            glfwTerminate();
            return false;
        }
        glfwSetWindowUserPointer(window, this);

        // Fullscreen On/Off
        monitor = glfwGetPrimaryMonitor(); // Get primary monitor
        mode = glfwGetVideoMode(monitor); // Get resolution of the monitor

        // Setup callbacks
        glfwMakeContextCurrent(window);
        glfwSetKeyCallback(window, key_callback);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetScrollCallback(window, scroll_callback);

        // Set V-Sync OFF.
        glfwSwapInterval(0);

        // Set V-Sync ON.
        //glfwSwapInterval(1);
        //isVsyncOn = true;

        // Init GLEW :: http://glew.sourceforge.net/basic.html
        GLenum err = glewInit();
        if (GLEW_OK != err) {
            fprintf(stderr, "Error: %s\n", glewGetErrorString(err)); /* Problem: glewInit failed, something is seriously wrong. */
        }
        wglewInit();

        //...after ALL GLFW & GLEW init ...
        if (GLEW_ARB_debug_output)
        {
            glDebugMessageCallback(MessageCallback, 0);
            glEnable(GL_DEBUG_OUTPUT);

            //default is asynchronous debug output, use this to simulate glGetError() functionality
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

            std::cout << "GL_DEBUG enabled.\n";
        }
        else
            std::cout << "GL_DEBUG NOT SUPPORTED!\n";
    }
    catch (std::exception const& e) {
        std::cerr << "Init failed : " << e.what() << "\n";
        throw;
    }
    std::cout << "Initialized...\n";
    return true;
}

int App::run(void)
{
    try {
        double fpsSecondsCounter = 0;
        int fpsFramesCounter = 0;

        while (!glfwWindowShouldClose(window))
        {
            // Time/FPS measure start
            auto fpsStart = std::chrono::steady_clock::now();

            // Clear OpenGL canvas, both color buffer and Z-buffer
            glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Swap front and back buffers
            glfwSwapBuffers(window);

            // Poll for and process events
            glfwPollEvents();

            // Time/FPS measure end
            auto fpsEnd = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed_seconds = fpsEnd - fpsStart;
            fpsSecondsCounter += elapsed_seconds.count();
            fpsFramesCounter++;
            if (fpsSecondsCounter >= 1) {
                //std::cout << fpsFramesCounter << " FPS\n";
                std::stringstream ss;
                ss << fpsFramesCounter << " FPS";
                glfwSetWindowTitle(window, ss.str().c_str());
                fpsSecondsCounter = 0;
                fpsFramesCounter = 0;
            }
        }
    }
    catch (std::exception const& e) {
        std::cerr << "App failed : " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    get_information();
    std::cout << "Finished OK...\n";
    return EXIT_SUCCESS;
}

App::~App()
{
    // clean-up
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();

    exit(EXIT_SUCCESS);
    std::cout << "Bye...\n";
}

void App::get_information() {
    std::cout << "\n============= :: GL Info :: =============\n";
    std::cout << "GL Vendor:\t" << glGetString(GL_VENDOR) << "\n";
    std::cout << "GL Renderer:\t" << glGetString(GL_RENDERER) << "\n";
    std::cout << "GL Version:\t" << glGetString(GL_VERSION) << "\n";
    std::cout << "GL Shading ver:\t" << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n\n";
    
    GLint profile;
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
    if (const auto errorCode = glGetError()) {
        std::cout << "Pending GL error while obtaining profile: " << errorCode << "\n";
        return;
    }
    if (profile & GL_CONTEXT_CORE_PROFILE_BIT) {
        std::cout << "Core profile" << "\n";
    }
    else {
        std::cout << "Compatibility profile" << "\n";
    }
    std::cout << "=========================================\n\n";
}

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
                glfwGetWindowSize(window, &win_width, &win_height);
                glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            }
            else {
                glfwSetWindowMonitor(window, nullptr, window_xcor, window_ycor, win_width, win_height, 0);
            }
            break;
        case GLFW_KEY_V:
            is_vsync_on = !is_vsync_on;
            glfwSwapInterval(is_vsync_on);
            std::cout << "VSync: " << is_vsync_on << "\n";
            break;
        default:
            break;
        }
    }
}

void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    auto this_instance = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (yoffset > 0.0) {
        //std::cout << "tocis nahoru...\n";
        this_instance->clear_color.r = std::clamp(this_instance->clear_color.r + 0.1f, 0.0f, 1.0f);
    }
    else if (yoffset < 0.0) {
        //std::cout << "tocis dolu...\n";
        this_instance->clear_color.r = std::clamp(this_instance->clear_color.r - 0.1f, 0.0f, 1.0f);
    }
}

void App::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void App::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        std::cout << "Right click!\n";
    }
}

void App::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    //std::cout << xpos << " " << ypos << "\n";
}
