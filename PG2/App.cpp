// Basic includes
#include <iostream>
#include <chrono>
#include <stack>
#include <random>
#include <sstream>

// OpenCV – GL independent
#include <opencv2/opencv.hpp>

// OpenGL Extension Wrangler: allow all multiplatform GL functions
#include <GL/glew.h> 
// WGLEW = Windows GL Extension Wrangler :: platform specific functions (in this case Windows)
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

#define print(x) std::cout << x << "\n"

// Static
bool App::is_vsync_on = false;
bool App::is_fullscreen_on = false;
GLFWmonitor* App::monitor;
const GLFWvidmode* App::mode;
int App::window_xcor{};
int App::window_ycor{};
int App::window_width = 1280;
int App::window_height = 800;
int App::window_width_return_from_fullscreen{};
int App::window_height_return_from_fullscreen{};

Camera App::camera = Camera(glm::vec3(0, 0, 1000));
double App::last_cursor_xpos{};
double App::last_cursor_ypos{};

AudioSlave App::audio;

int App::is_flashlight_on = 1;

App::App()
{
    std::cout << "Constructed...\n--------------\n";
}

// App initialization, if returns true then run run()
bool App::Init()
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
        glfwSetScrollCallback(window, scroll_callback);

        // Set V-Sync ON.
        glfwSwapInterval(1);
        is_vsync_on = true;

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
        else std::cout << "GL_DEBUG NOT SUPPORTED!\n";

        // Set GL params
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POLYGON_SMOOTH);

        glEnable(GL_CULL_FACE); // assuming ALL objects are non-transparent 

        // Transparency blending function
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // First init OpenGL, THAN init assets: valid context MUST exist
        InitAssets();
    }
    catch (std::exception const& e) {
        std::cerr << "Init failed : " << e.what() << "\n";
        exit(-1);
    }
    std::cout << "--------------\nInitialized...\n";
    return true;
}

int App::Run(void)
{
    try {
        double fps_counter_seconds = 0;
        int fps_counter_frames = 0;

        UpdateProjectionMatrix();
        glViewport(0, 0, window_width, window_height);
        camera.position = glm::vec3(0, 0, 0);
        double last_frame_time = glfwGetTime();
        glm::vec3 camera_movement{};

        // Set camera position
        camera.position.y = 2.0f;
        camera.position.z = 5.0f;

        // Music
        audio.PlayMusic3D();

        // Misc
        glm::vec3 ball_movement{};

        // Main loop
        while (!glfwWindowShouldClose(window)) {
            // Time/FPS measure start
            auto fps_frame_start_timestamp = std::chrono::steady_clock::now();

            // Clear OpenGL canvas, both color buffer and Z-buffer
            glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // === After clearing the canvas ===

            // React to user :: Create View Matrix according to camera settings
            float delta_time = static_cast<float>(glfwGetTime() - last_frame_time);
            last_frame_time = glfwGetTime();
            camera_movement = camera.ProcessInput(window, delta_time);
            camera.position += camera_movement;
            glm::mat4 mx_view = camera.GetViewMatrix();
            
            // 3D Audio
            camera.UpdateListenerPosition(audio);

            // Misc Input
            ball_movement = BallMovement(delta_time);
            ball_position += ball_movement;

            // Set Model Matrix
            UpdateModels();

            // Activate shader
            my_shader.Activate();

            // Set shader uniform variables
            my_shader.SetUniform("u_mx_view", mx_view); // World space -> Camera space
            my_shader.SetUniform("u_mx_projection", mx_projection); // Camera space -> Screen

            // UBER
            my_shader.SetUniform("u_ambient_alpha", 0.0f);
            my_shader.SetUniform("u_diffuse_alpha", 0.66f);
            my_shader.SetUniform("u_specular_alpha", 0.0f);
            my_shader.SetUniform("u_camera_position", camera.position);

            // - AMBIENT
            my_shader.SetUniform("u_material.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
            my_shader.SetUniform("u_material.specular", glm::vec3(0.7f, 0.7f, 0.7f));
            my_shader.SetUniform("u_material.shininess", 96.0f);

            // - DIRECTION (SUN O))))
            my_shader.SetUniform("u_directional_light.direction", glm::vec3(0.0f, -0.9f, -0.17f));
            my_shader.SetUniform("u_directional_light.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
            my_shader.SetUniform("u_directional_light.specular", glm::vec3(0.2f, 0.2f, 0.2f));

            // - POINT LIGHT #0 :: GREEN-ISH
            my_shader.SetUniform("u_point_lights[0].diffuse", glm::vec3(0.0f, 1.0f, 0.1f));
            my_shader.SetUniform("u_point_lights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
            my_shader.SetUniform("u_point_lights[0].position", ball_position);
            //my_shader.SetUniform("u_point_lights[0].position", glm::vec3(0.0f, 0.0f, 0.0f));
            my_shader.SetUniform("u_point_lights[0].constant", 1.0f);
            my_shader.SetUniform("u_point_lights[0].linear", 0.22f);
            my_shader.SetUniform("u_point_lights[0].exponent", 0.20f);

            // - SPOTLIGHT
            my_shader.SetUniform("u_spotlight.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
            my_shader.SetUniform("u_spotlight.specular", glm::vec3(0.9f, 0.9f, 0.9f));
            my_shader.SetUniform("u_spotlight.position", camera.position);
            my_shader.SetUniform("u_spotlight.direction", camera.front);
            my_shader.SetUniform("u_spotlight.cos_inner_cone", glm::cos(glm::radians(20.0f)));
            my_shader.SetUniform("u_spotlight.cos_outer_cone", glm::cos(glm::radians(27.0f)));
            my_shader.SetUniform("u_spotlight.constant", 1.0f);
            my_shader.SetUniform("u_spotlight.linear", 0.07f);
            my_shader.SetUniform("u_spotlight.exponent", 0.017f);
            my_shader.SetUniform("u_spotlight.on", is_flashlight_on);
            
            // Draw the scene
            // - Draw opaque objects
            for (auto& [key, value] : scene_opaque) {
                value.Draw(my_shader);
            }
            // - Draw transparent objects
            glEnable(GL_BLEND);         // enable blending
            glDisable(GL_CULL_FACE);    // no polygon removal
            glDepthMask(GL_FALSE);      // set Z to read-only
            // TODO (FRFR): sort by distance from camera, from far to near
            for (auto& [key, value] : scene_transparent) {
                value.Draw(my_shader);
            }
            glDisable(GL_BLEND);
            glEnable(GL_CULL_FACE);
            glDepthMask(GL_TRUE);

            // === End of frame ===
            // Swap front and back buffers
            glfwSwapBuffers(window);

            // Poll for and process events
            glfwPollEvents();
            
            // Time/FPS measure end
            auto fps_frame_end_timestamp = std::chrono::steady_clock::now();
            std::chrono::duration<double> fps_elapsed_seconds = fps_frame_end_timestamp - fps_frame_start_timestamp;
            fps_counter_seconds += fps_elapsed_seconds.count();
            fps_counter_frames++;
            if (fps_counter_seconds >= 1) {
                std::stringstream ss;
                ss << fps_counter_frames << " FPS | " << FOV << " FOV";
                glfwSetWindowTitle(window, ss.str().c_str());
                fps_counter_seconds = 0;
                fps_counter_frames = 0;
            }
        }
    }
    catch (std::exception const& e) {
        std::cerr << "App failed : " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    GetInformation();
    std::cout << "Finished OK...\n";
    return EXIT_SUCCESS;
}

App::~App()
{
    // clean-up
    my_shader.Clear();

    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();

    exit(EXIT_SUCCESS);
    std::cout << "Bye...\n";
}

void App::UpdateProjectionMatrix(void)
{
    if (window_height < 1) window_height = 1; // avoid division by 0

    float ratio = static_cast<float>(window_width) / window_height;

    mx_projection = glm::perspective(
        glm::radians(FOV),   // The vertical Field of View
        ratio,               // Aspect Ratio. Depends on the size of your window.
        0.1f,                // Near clipping plane. Keep as big as possible, or you'll get precision issues.
        20000.0f             // Far clipping plane. Keep as little as possible.
    );
}

void App::GetInformation()
{
    std::cout << "\n=================== :: GL Info :: ===================\n";
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
    std::cout << "=====================================================\n\n";
}
