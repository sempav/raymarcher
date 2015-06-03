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
    static App *instance;

	Window window;

	float last_render_time;
	bool init_ok;


    std::unique_ptr<GLProgram> program;

	Quad *quad;

	float cur_camera_acceleration;
	SmoothCamera *camera;

	bool InitGL();
	bool InitObjects();

	void ProcessInput();

    bool OnInit(int argc, char *argv[]);
    void OnEvent();
    void OnResize(int width, int height);
    void OnKey(int key, int scancode, int action, int mode);
    void OnCursorPos(double xpos, double ypos);
    void OnLoop();
    void OnRender();
    void OnCleanup();

    App(const App &other) = delete;
    App& operator= (const App &other) = delete;

    App();
	~App();

public:
    static App &GetInstance() {
        if (!instance) {
            instance = new App();
        }
        return *instance;
    }

    static void ResetInstance() {
        if (instance) {
            delete instance;
            instance = nullptr;
        }
    }

	bool Initialize();
    int OnExecute(int argc, char *argv[]);

    KeyboardListener keys;

    friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    friend void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    friend void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
};
