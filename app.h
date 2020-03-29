#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "defines.h"
#include "keyboard.h"
#include "logger.h"
#include "quad.h"
#include "shader.h"
#include "window.h"

class App
{
    Window window;

    float last_render_time;
    bool init_ok;

    std::unique_ptr<GLProgram> program;
    std::unique_ptr<Quad> quad;

    float cur_camera_acceleration;
    std::unique_ptr<SmoothCamera> camera;

    bool Initialize(std::string vertex_path, std::string fragment_path);
    bool InitGL(std::string vertex_path, std::string fragment_path);

    void ProcessInput();

    void OnResize(int width, int height);
    void OnKey(int key, int scancode, int action, int mode);
    void OnCursorPos(double xpos, double ypos);
    void OnLoop();
    void OnRender();
    void OnCleanup();

    App(const App &other) = delete;
    App& operator= (const App &other) = delete;

    App();

public:
    static App &GetInstance() {
        static App instance;
        return instance;
    }

    int OnExecute(std::string vertex_path, std::string fragment_path);

    KeyboardListener keys;

    friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    friend void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    friend void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
};
