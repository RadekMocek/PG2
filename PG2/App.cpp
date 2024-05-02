// C++
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
        
        // Window is hidden until everything is initialized
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        // Open window (GL canvas) with no special properties :: https://www.glfw.org/docs/latest/quick.html#quick_create_window
        window = glfwCreateWindow(window_width, window_height, "Moje krasne okno", NULL, NULL);
        if (!window) {
            glfwTerminate();
            return false;
        }
        glfwSetWindowUserPointer(window, this);

        // Hide cursor
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // <- weird mouselook behavior
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // <- ok

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
        
        //PrintGLInfo();
        
        // First init OpenGL, THAN init assets: valid context MUST exist
        InitAssets();

        // Show window after everything loads        
        glfwShowWindow(window);
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
        camera.position.x = 1.0f;
        camera.position.y = 1.0f;
        camera.position.z = 1.0f;

        // Mouselook
        double cursor_x, cursor_y;

        // Heightmap _heights reference
        auto& _heights = obj_heightmap->_heights;

        // Jetpack
        float falling_speed = 0;

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
            float delta_time = static_cast<float>(glfwGetTime() - last_frame_time);
            last_frame_time = glfwGetTime();
            
            // Player movement
            camera_movement = camera.ProcessInput(window, delta_time);
            camera.position.x += camera_movement.x;
            camera.position.z += camera_movement.z;

            // Mouselook
            if (is_mouselook_on) {
                glfwGetCursorPos(window, &cursor_x, &cursor_y);
                camera.ProcessMouseMovement(static_cast<GLfloat>(window_width / 2.0 - cursor_x), static_cast<GLfloat>(window_height / 2.0 - cursor_y));
                glfwSetCursorPos(window, window_width / 2.0, window_height / 2.0);
            }

            // Heightmap collision :: https://textbooks.cs.ksu.edu/cis580/15-heightmap-terrain/05-interpolating-heights/index.html
            float hm_x_f = camera.position.x + HEIGHTMAP_SHIFT;
            float hm_z_f = camera.position.z + HEIGHTMAP_SHIFT;
            float hm_y_f = 0.0f;
            float hm_x_i = std::floor(hm_x_f);
            float hm_z_i = std::floor(hm_z_f);
            if (hm_x_f - hm_x_i < 0.5f && hm_z_f - hm_z_i < 0.5f) {
                // In the lower-left triangle
                float x_fraction = hm_x_f - hm_x_i;
                float y_fraction = hm_z_f - hm_z_i;
                float common_height = _heights[{hm_x_i, hm_z_i}];
                float x_difference = _heights[{hm_x_i + 1, hm_z_i}] - common_height;
                float y_difference = _heights[{hm_x_i, hm_z_i + 1}] - common_height;
                hm_y_f = common_height + x_fraction * x_difference + y_fraction * y_difference;
            }
            else {
                // In the upper-right triangle
                float x_fraction = hm_x_i + 1 - hm_x_f;
                float y_fraction = hm_z_i + 1 - hm_z_f;
                float common_height = _heights[{hm_x_i + 1, hm_z_i + 1}];
                float x_difference = common_height - _heights[{hm_x_i, hm_z_i + 1}];
                float y_difference = common_height - _heights[{hm_x_i + 1, hm_z_i}];
                hm_y_f = common_height - x_fraction * x_difference - y_fraction * y_difference;
            }

            // Jetpack
            float min_hei = hm_y_f * HEGHTMAP_SCALE + PLAYER_HEIGHT;
            if (camera_movement.y > 0.0f) {
                camera.position.y += delta_time * 2.0f;
                falling_speed = 0;
                if (camera.position.y < min_hei) {
                    camera.position.y = min_hei;
                }
            }
            else {
                falling_speed += delta_time * 9.81f;
                camera.position.y -= delta_time * falling_speed;
                if (camera.position.y < min_hei) {
                    camera.position.y = min_hei;
                    falling_speed = 0;
                }
            }

            // Create View Matrix according to camera settings
            glm::mat4 mx_view = camera.GetViewMatrix();
            
            // 3D Audio
            camera.UpdateListenerPosition(audio, camera.position - obj_jukebox->position);

            // Update objects
            jukebox_to_player.x = camera.position.x - obj_jukebox->position.x;
            jukebox_to_player.y = camera.position.z - obj_jukebox->position.z;
            jukebox_to_player_n = glm::normalize(jukebox_to_player);
            UpdateModels(delta_time);
            UpdateProjectiles(delta_time);

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

            // - POINT LIGHT :: JUKEBOX
            my_shader.SetUniform("u_point_lights[0].diffuse", glm::vec3(0.0f, 0.8f, 0.8f));
            my_shader.SetUniform("u_point_lights[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
            glm::vec3 point_light_pos = obj_jukebox->position;
            point_light_pos.y += 1.0f;
            point_light_pos.x += 0.7f * jukebox_to_player_n.x;
            point_light_pos.z += 0.7f * jukebox_to_player_n.y;
            my_shader.SetUniform("u_point_lights[0].position", point_light_pos);
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
                value->Draw(my_shader);
            }
            // - Draw transparent objects
            glEnable(GL_BLEND);         // enable blending
            glDisable(GL_CULL_FACE);    // no polygon removal
            glDepthMask(GL_FALSE);      // set Z to read-only
            // - - Calculate distace from camera for all transparent objects
            for (auto& transparent_pair : scene_transparent_pairs) {
                transparent_pair->second->_distance_from_camera = glm::length(camera.position - transparent_pair->second->position);
            }
            // - - Sort all transparent objects in vector by their distance from camera (far to near)
			std::sort(scene_transparent_pairs.begin(), scene_transparent_pairs.end(), [](std::pair<const std::string, Model*>*& a, std::pair<const std::string, Model*>*& b) {
				return a->second->_distance_from_camera > b->second->_distance_from_camera;
			});
            // - - Draw all transparent objects in sorted order
            for (auto& transparent_pair : scene_transparent_pairs) {
                transparent_pair->second->Draw(my_shader);
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
                FPS = fps_counter_frames;
                fps_counter_seconds = 0;
                fps_counter_frames = 0;
            }
            // Window title
            std::stringstream ss;
            ss << FPS << " FPS | " << FOV << " FOV | X" << camera.position.x << " Z" << camera.position.z;
            glfwSetWindowTitle(window, ss.str().c_str());
        }
    }
    catch (std::exception const& e) {
        std::cerr << "App failed : " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    
    PrintGLInfo();

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

void App::PrintGLInfo()
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
