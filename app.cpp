#include "app.h"

#include <chrono>
#include <thread>

void key_callback(GLFWwindow*, int key, int scancode, int action, int mods) {
    App::GetInstance().OnKey(key, scancode, action, mods);
}

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    App::GetInstance().OnResize(width, height);
}

void cursor_position_callback(GLFWwindow*, double xpos, double ypos) {
    App::GetInstance().OnCursorPos(xpos, ypos);
}

App::App(void)
    : window(SCREEN_WIDTH, SCREEN_HEIGHT, "Ima title"),
      last_frame_duration(0),
      init_ok(false),
      program(),
      cur_camera_acceleration(CAMERA_ACCELERATION),
      camera(std::make_unique<SmoothCamera>(glm::vec3(0.00, 0.0, -5.00001),
                                            glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0))) {
    glfwSetKeyCallback(window.handle, key_callback);
    glfwSetFramebufferSizeCallback(window.handle, framebuffer_size_callback);
    glfwSetCursorPosCallback(window.handle, cursor_position_callback);

    glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glViewport(0, 0, window.width, window.height);
}

bool App::Initialize(std::string vertex_path, std::string fragment_path) {
    assert(!init_ok);

    if (!InitGL(vertex_path, fragment_path)) {
        return false;
    }

    quad = std::make_unique<Quad>();
    if (!quad->LoadVBO()) {
        quad.reset();
        camera.reset();
        program.reset();

        throw std::runtime_error("Failed to load quad VBO");
    }
    init_ok = true;
    logger.Write("App initialized.\n");
    return init_ok;
}

bool App::InitGL(std::string vertex_path, std::string fragment_path) {
    program.reset(new GLProgram());

    std::unique_ptr<GLShader> vertex(new GLShader(GL_VERTEX_SHADER));
    vertex->LoadFromFile(vertex_path.c_str());
    program->LoadVertexShader(vertex.release());

    std::unique_ptr<GLShader> fragment(new GLShader(GL_FRAGMENT_SHADER));
    fragment->LoadFromFile(fragment_path.c_str());
    program->LoadFragmentShader(fragment.release());

    if (!program->Link()) {
        program.reset();
        return false;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);

    glEnable(GL_CULL_FACE);

    logger.Write("Program loaded.\n");
    return true;
}

void App::OnResize(int width, int height) {
    glViewport(0, 0, width, height);
    window.width = width;
    window.height = height;
    window.ratio = width / static_cast<float>(height);
}

void App::OnKey(int key, int scancode, int action, int mode) {
    if (action == GLFW_PRESS) {
        keys.PressKey(key);
    } else if (action == GLFW_RELEASE) {
        keys.ReleaseKey(key);
    }
}

void App::OnCursorPos(double xpos, double ypos) {
    static const float ROT_COEFF = MOUSE_SENSITIVITY_PER_SEC * last_frame_duration.count();
    static double old_xpos = 0.5;
    static double old_ypos = 0.5;
    camera->RotateLoc(ROT_COEFF, old_ypos - ypos, old_xpos - xpos, 0.0f);
    old_xpos = xpos;
    old_ypos = ypos;
}

void App::OnLoop() { ProcessInput(); }

void App::OnRender() {
    static auto prev_time = std::chrono::steady_clock::now();
    auto current_time = std::chrono::steady_clock::now();

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program->Use();

    program->SetCamera(*camera, window.ratio);
    program->SetUniform1f("elapsed_time",
                          std::chrono::duration<float>(current_time.time_since_epoch()).count());

    quad->Draw(program.get());

    glfwSwapBuffers(window.handle);

    last_frame_duration = std::chrono::duration<float>(current_time - prev_time);
    prev_time = current_time;

    fps_counter.RegisterFrame(last_frame_duration);
    char s[50];
    sprintf(s, "%.1f fps (%.4f sec/frame)", fps_counter.GetAvgFps(),
            fps_counter.GetAvgTime().count());
    glfwSetWindowTitle(window.handle, s);
}

int App::OnExecute(std::string vertex_path, std::string fragment_path) {
    if (!Initialize(vertex_path, fragment_path)) {
        return EXIT_FAILURE;
    }
    constexpr auto ms_per_frame = std::chrono::duration<double, std::milli>(1000) / FRAMERATE;
    while (!glfwWindowShouldClose(window.handle)) {
        auto time_start = std::chrono::steady_clock::now();

        glfwPollEvents();
        OnLoop();
        OnRender();

        auto time_end = std::chrono::steady_clock::now();
        auto time_delta = std::chrono::duration<double, std::milli>(time_end - time_start);
        if (time_delta < ms_per_frame) {
            std::this_thread::sleep_for(ms_per_frame - time_delta);
        }
    }
    return EXIT_SUCCESS;
}

void App::ProcessInput() {
    float ROLL_ANGLE = ROLL_ANGLE_PER_SEC * last_frame_duration.count();
    if (keys[GLFW_KEY_ESCAPE]) {
        glfwSetWindowShouldClose(window.handle, GL_TRUE);
    }
    if (keys[GLFW_KEY_W]) {
        camera->Accelerate(glm::vec3(0.0f, 0.0f, cur_camera_acceleration));
    }
    if (keys[GLFW_KEY_S]) {
        camera->Accelerate(glm::vec3(0.0f, 0.0f, -cur_camera_acceleration));
    }
    if (keys[GLFW_KEY_A]) {
        camera->Accelerate(glm::vec3(-cur_camera_acceleration, 0.0f, 0.0f));
    }
    if (keys[GLFW_KEY_D]) {
        camera->Accelerate(glm::vec3(cur_camera_acceleration, 0.0f, 0.0f));
    }
    if (keys[GLFW_KEY_Q]) {
        camera->RotateLoc(ROLL_ANGLE, 0.0f, 0.0f, -1.0f);
    }
    if (keys[GLFW_KEY_E]) {
        camera->RotateLoc(ROLL_ANGLE, 0.0f, 0.0f, 1.0f);
    }
    keys.ClearNewKeys();
    camera->Update();
}
