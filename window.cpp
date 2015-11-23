#include "window.h"

#include "logger.h"

#include <stdexcept>
#include <GL/glew.h>

Window::Window(int w, int h, std::string title): handle(nullptr),
                                                 width(w), height(h),
                                                 ratio((1.0 * width) / height)
{
    // prevents tiling wms (such as i3) from tiling the window,
    // and makes it floating instead
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_FOCUSED, GL_TRUE);

    handle = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!handle) {
        throw std::runtime_error("Failed to create window");
    }
    glfwMakeContextCurrent(handle);

    glewExperimental = GL_TRUE;
    GLenum glew_status = glewInit();
    if (glew_status != GLEW_OK) {
        logger.Write("Error initializing GLEW: %s\n", glewGetErrorString(glew_status));
        throw std::runtime_error("Failed to initialize GLEW");
    }

    if (!GLEW_VERSION_3_3) {
        throw std::runtime_error("OpenGL 3.3 not supported");
    }

}

Window::~Window()
{
    glfwDestroyWindow(handle);
}
