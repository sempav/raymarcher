#include "defines.h"
#include "logger.h"
#include "app.h"
#include "shader.h"

#include <iostream>

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s", error, description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    App::GetInstance().OnKey(key, scancode, action, mods);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    App::GetInstance().OnResize(width, height);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    App::GetInstance().OnCursorPos(xpos, ypos);
}

bool parse_args(int argc, char **argv,
                std::string &vertex_path, std::string &fragment_path)
{
    vertex_path = "shaders/vertex.glsl";
    fragment_path = "shaders/fragment.glsl";
    int cur = 1;
    while (argc >= cur + 2) {
        if (std::string(argv[cur]) == "-v") {
            vertex_path = argv[cur + 1];
        } else if (std::string(argv[cur]) == "-f") {
            fragment_path = argv[cur + 1];
        } else {
            return false;
        }
        cur += 2;
    }
    if (argc > cur) {
        return false;
    }
    return true;
}

void print_usage(int argc, char **argv)
{
    std::cout << "Usage: " << (argc ? argv[0] : "main") << " ";
    std::cout << "[-f fragment_path] [-v vertex_path]";
    std::cout << std::endl;
}

int main(int argc, char **argv)
{
    logger.LogStartup(argc, argv);

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        return EXIT_FAILURE;
    }

    std::string vertex_path, fragment_path;
    if (!parse_args(argc, argv, vertex_path, fragment_path)) {
        print_usage(argc, argv);
        return EXIT_FAILURE;
    }

    App &app = App::GetInstance();

    logger.LogSystemInfo();

    try {
        return app.OnExecute(vertex_path, fragment_path);
    } catch(const std::exception &e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        std::cerr << "Program terminated." << std::endl;
    }
}
