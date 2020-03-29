#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

struct Window {
    GLFWwindow *handle;
    int width;
    int height;
    float ratio;

    Window(int w, int h, std::string title);
    ~Window();
};
