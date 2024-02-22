class App {
public:
    App();

    bool init(void);
    int run(void);

    ~App();
private:
    GLFWwindow* window = nullptr;

    void getInformation();

    static void error_callback(int error, const char* description);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};