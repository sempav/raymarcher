#include "app.h"

#include <chrono>
#include <thread>

void error_callback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

App *App::instance = nullptr;

App::App(void) :
    window(SCREEN_WIDTH, SCREEN_HEIGHT, "Ima title"), 
    last_render_time(0), init_ok(false),
    program(),
    quad(NULL),
    cur_camera_acceleration(CAMERA_ACCELERATION),
    camera(new SmoothCamera(glm::vec3(0.00, 0.0, -5.00001), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0)))
{
    glfwSetKeyCallback(window.handle, key_callback);
    glfwSetFramebufferSizeCallback(window.handle, framebuffer_size_callback);
    glfwSetCursorPosCallback(window.handle, cursor_position_callback);

    glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glViewport(0, 0, window.width, window.height);
}

App::~App(void)
{
    if (init_ok) {
        logger.Write("App destroyed.\n");
        delete quad;
        delete camera;
    }
    glfwTerminate();
}

bool App::Initialize(std::string vertex_path, std::string fragment_path)
{
    assert(!init_ok);

    if (!InitGL(vertex_path, fragment_path)) {
        return false;
    }

    quad = new Quad();
    if (!quad->LoadVBO()) {
        delete quad;
        quad = NULL;
        delete camera;
        camera = NULL;
        program.reset();

        throw std::runtime_error("Failed to load quad VBO");
    }
    init_ok = true;
    logger.Write("App initialized.\n");
    return init_ok;
}

bool App::InitGL(std::string vertex_path, std::string fragment_path)
{
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

bool App::InitObjects()
{
    logger.Write("Initializing objects..\n");
    logger.Write("All objects initialized.\n");
    return true;
}

void App::OnEvent()
{
}

void App::OnResize(int width, int height)
{
    glViewport(0, 0, width, height);
    window.width = width;
    window.height = height;
    window.ratio = width / static_cast<float>(height);
}

void App::OnKey(int key, int scancode, int action, int mode)
{
    if (action == GLFW_PRESS) {
        keys.PressKey(key);
    } else if (action == GLFW_RELEASE) {
        keys.ReleaseKey(key);
    }
}

void App::OnCursorPos(double xpos, double ypos)
{
    static const float ROT_COEFF = MOUSE_SENSITIVITY_PER_SEC * last_render_time;
    static double old_xpos = 0.5;
    static double old_ypos = 0.5;
    camera->RotateLoc(ROT_COEFF, old_ypos - ypos, old_xpos - xpos, 0.0f);
    old_xpos = xpos;
    old_ypos = ypos;
}

void App::OnLoop()
{
    ProcessInput();
}

void App::OnRender()
{
    assert(quad);
    assert(camera);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program->Use();

    program->SetCamera(*camera, window.ratio);
    float elapsed_time = glfwGetTime();
    program->SetUniform1f("elapsed_time", elapsed_time);

    quad->Draw(program.get());

    glfwSwapBuffers(window.handle);

    static float t = 0;
    last_render_time = elapsed_time - t;
    t  = elapsed_time;
    char s[50];
    sprintf(s, "%.2f fps (%.4f sec/frame)",
                           1.0 / last_render_time,
                           last_render_time);

    glfwSetWindowTitle(window.handle, s);

    if (last_render_time < 1e-6) last_render_time = 1e-6;
}

int App::OnExecute(std::string vertex_path, std::string fragment_path)
{
    if (!Initialize(vertex_path, fragment_path)) {
        return -1;
    }
    int time_start, time_delta;
    while (!glfwWindowShouldClose(window.handle)) {
        time_start = static_cast<int>(1000 * glfwGetTime());
        glfwPollEvents(); // do callbacks execute in this thread?
        OnLoop();
        OnRender();

        time_delta = static_cast<int>(1000 * glfwGetTime()) - time_start;
        if (time_delta < MSEC_PER_FRAME) {
            std::this_thread::sleep_for(std::chrono::milliseconds(MSEC_PER_FRAME - time_delta));
        }
    }
    return 0;
} 

void App::ProcessInput()
{
    float ROLL_ANGLE = ROLL_ANGLE_PER_SEC * last_render_time;
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

