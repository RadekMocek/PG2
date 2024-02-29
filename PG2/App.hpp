class App {
public:
    App();

    bool init(void);
    int run(void);

    ~App();
private:
    static bool is_vsync_on;
    static bool is_fullscreen_on;
    
    static GLFWmonitor* monitor;
    static const GLFWvidmode* mode;
    static int win_xcor;
    static int win_ycor;
    static int win_wid;
    static int win_hei;

    GLFWwindow* window = nullptr;
    glm::vec4 clear_color = glm::vec4(0, 0, 0, 0);

    void get_information();

    static void error_callback(int error, const char* description);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};