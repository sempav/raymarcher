#include "defines.h"
#include "logger.h"
#include "app.h"
#include "shader.h"

#include <iostream>

App *app_ptr = nullptr;

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s", error, description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    app_ptr->OnKey(key, scancode, action, mods);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    app_ptr->OnResize(width, height);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    app_ptr->OnCursorPos(xpos, ypos);
}

int main(int argc, char **argv)
{
    logger.LogStartup(argc, argv);

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        return EXIT_FAILURE;
    }

    App app;
    app_ptr = &app;

    logger.LogSystemInfo();

    return app.OnExecute(argc, argv);
}
