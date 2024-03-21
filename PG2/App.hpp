#include <unordered_map>

#include "Model.hpp"
#include "ShaderProgram.hpp"
#include "Camera.hpp"

class App {
public:
    App();

    bool init();
    void init_assets();
    int run();

    ~App();
protected:
    std::unordered_map<std::string, Model> scene;
private:
    static bool is_vsync_on;
    static bool is_fullscreen_on;
    
    static GLFWmonitor* monitor;
    static const GLFWvidmode* mode;
    static int window_xcor;
    static int window_ycor;
    static int window_width;
    static int window_height;

    float FOV = 89.0f;
    glm::mat4 projection_matrix = glm::identity<glm::mat4>();
    //Camera camera;

    GLFWwindow* window = nullptr;
    glm::vec4 clear_color = glm::vec4(0, 0, 0, 0);

    void update_projection_matrix();

    void get_information();

    static void error_callback(int error, const char* description);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    ShaderProgram my_shader;
};