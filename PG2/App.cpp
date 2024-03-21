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
#include "ShaderProgram.hpp"

bool App::is_vsync_on = false;
bool App::is_fullscreen_on = false;
GLFWmonitor* App::monitor;
const GLFWvidmode* App::mode;
int App::window_xcor{};
int App::window_ycor{};
int App::window_width = 800;
int App::window_height = 600;

App::App()
{
    // default constructor
    // nothing to do here (for now...)
    std::cout << "Constructed...\n";
}

void App::init_assets()
{
    // load models, load textures, load shaders, initialize level, etc...
    std::filesystem::path VS_path("./resources/basic.vert");
    std::filesystem::path FS_path("./resources/basic.frag");
    my_shader = ShaderProgram(VS_path, FS_path);

    //std::filesystem::path model_path("./resources/objects/bunny_tri_vn.obj");
    //std::filesystem::path model_path("./resources/objects/bunny_tri_vnt.obj");
    //std::filesystem::path model_path("./resources/objects/cube_triangles.obj");
    //std::filesystem::path model_path("./resources/objects/cube_triangles_normals_tex.obj");
    //std::filesystem::path model_path("./resources/objects/plane_tri_vnt.obj");
    std::filesystem::path model_path("./resources/objects/sphere_tri_vnt.obj");
    //std::filesystem::path model_path("./resources/objects/teapot_tri_vnt.obj");
    Model my_model = Model(model_path);    
    
    scene.insert({ "obj_test", my_model });
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
        window = glfwCreateWindow(window_width, window_height, "Moje krasne okno", NULL, NULL);
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
        glfwSetScrollCallback(window, scroll_callback);                     // Mousewheel

        // Set V-Sync OFF.
        //glfwSwapInterval(0);

        // Set V-Sync ON.
        ///*
        glfwSwapInterval(1);
        is_vsync_on = true;
        /**/

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

        // set GL params
        glEnable(GL_DEPTH_TEST);
        // first init OpenGL, THAN init assets: valid context MUST exist
        init_assets();
    }
    catch (std::exception const& e) {
        std::cerr << "Init failed : " << e.what() << "\n";
        //throw;
        exit(-1);
    }
    std::cout << "Initialized...\n";
    return true;
}

int App::run(void)
{
    try {
        double fpsSecondsCounter = 0;
        int fpsFramesCounter = 0;

        glm::vec4 my_rgba = { 1.0f, 0.5f, 0.0f, 1.0f };

        while (!glfwWindowShouldClose(window))
        {
            // Time/FPS measure start
            auto fpsStart = std::chrono::steady_clock::now();

            // Clear OpenGL canvas, both color buffer and Z-buffer
            glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Ater clearing canvas
            my_shader.activate();
            my_shader.setUniform("myrgba", my_rgba);
            for (auto const& model_pair : scene) {
                auto model = model_pair.second;
                model.Draw(my_shader);
            }

            // End of frame
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
    my_shader.clear();

    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();

    exit(EXIT_SUCCESS);
    std::cout << "Bye...\n";
}

void App::update_projection_matrix(void)
{
    if (window_height < 1)
        window_height = 1;   // avoid division by 0

    float ratio = static_cast<float>(window_width) / window_height;

    projection_matrix = glm::perspective(
        glm::radians(FOV),   // The vertical Field of View
        ratio,               // Aspect Ratio. Depends on the size of your window.
        0.1f,                // Near clipping plane. Keep as big as possible, or you'll get precision issues.
        20000.0f             // Far clipping plane. Keep as little as possible.
    );
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
        std::cout << "[!] Pending GL error while obtaining profile: " << errorCode << "\n";
        //return;
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
                glfwGetWindowSize(window, &window_width, &window_height);
                if (window_height == 0) window_height++;
                glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            }
            else {
                glfwSetWindowMonitor(window, nullptr, window_xcor, window_ycor, window_width, window_height, 0);
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
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    this_inst->FOV += 10 * yoffset;
    this_inst->FOV = std::clamp(this_inst->FOV, 20.0f, 170.0f); // limit FOV to reasonable values...
    this_inst->update_projection_matrix();
}

void App::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    this_inst->window_width = width;
    this_inst->window_height = height;
    // set viewport
    glViewport(0, 0, width, height);
    //now your canvas has [0,0] in bottom left corner, and its size is [width x height] 
    this_inst->update_projection_matrix();
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
