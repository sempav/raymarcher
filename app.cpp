#include "app.h"

App::App(void) :
    window(SCREEN_WIDTH, SCREEN_HEIGHT), 
    last_render_time(0), init_ok(false),
    program(),
    quad(NULL),
    cur_camera_acceleration(CAMERA_ACCELERATION),
    camera(NULL),
    keys()
{
}

App::~App(void)
{
    if (init_ok) {
        logger.Write("App destroyed.\n");
        delete quad;
        delete camera;
    }
}

bool App::Initialize()
{
    assert(!init_ok);

    if (!InitGL()) {
        return false;
    }

    camera = new SmoothCamera(glm::vec3(0.00, 0.0, -5.00001), glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 0.0));

    if (!InitObjects()) {
        delete camera;
        camera = NULL;
        program.reset();
        return false;
    }
    init_ok = true;
    logger.Write("App initialized.\n");
    return init_ok;
}

bool App::InitGL()
{
    program.reset(new GLProgram());

    std::unique_ptr<GLShader> vertex(new GLShader(GL_VERTEX_SHADER));
    vertex->LoadFromFile("shaders/vertex.glsl");
    program->LoadVertexShader(vertex.release());

    std::unique_ptr<GLShader> fragment(new GLShader(GL_FRAGMENT_SHADER));
    fragment->LoadFromFile("shaders/fragment.glsl");
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
    quad = new Quad();
    if (!quad->LoadVBO()) {
        delete quad;
        quad = NULL;
        return false;
    }
    logger.Write("All objects initialized.\n");
    return true;
}

void drawText(float x, float y, void *font, char *string) {
    char *c;
    glRasterPos2f(x,y);

    for(c=string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void App::onDisplay()
{
    assert(quad);
    assert(camera);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program->Use();

    //glm::mat4 model = glm::mat4(1.0f);
    //glm::mat4 view = camera->GetViewMatrix();
    //glm::mat4 projection = glm::perspective(45.0f, 1.0f * window.width / window.height, 0.001f, 100.0f);
    //glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
    //program->SetUniformMat4("model", 1, GL_FALSE, glm::value_ptr(model));
    //program->SetUniformMat4("view", 1, GL_FALSE, glm::value_ptr(view));
    //program->SetUniformMat4("projection", 1, GL_FALSE, glm::value_ptr(projection));

    // program->SetUniform3fv("camera_pos", 1, glm::value_ptr(camera->GetPos()));

    program->SetCamera(*camera);
    float elapsed_time = 0.001 * glutGet(GLUT_ELAPSED_TIME);
    program->SetUniform1f("elapsed_time", elapsed_time);

    quad->Draw(program.get());

    glutSwapBuffers();

    //drawText(0.2f, 0.5, GLUT_BITMAP_HELVETICA_18, "Yo!01Il1ij!,.");

    static float t = 0;
    last_render_time = elapsed_time - t;
    t  = elapsed_time;
    char s[50];
    sprintf(s, "%.2f fps (%.4f sec/frame)",
                           1.0 / last_render_time,
                           last_render_time);
    glutSetWindowTitle(s);

    if (last_render_time < 1e-6) last_render_time = 1e-6;
}

void App::ProcessInput()
{
    float ROLL_ANGLE = ROLL_ANGLE_PER_SEC * last_render_time;
    if (keys[27]) // Escape
        glutLeaveMainLoop();
    if (keys['w'])
        camera->Accelerate(glm::vec3(0.0f, 0.0f, cur_camera_acceleration));
    if (keys['s'])
        camera->Accelerate(glm::vec3(0.0f, 0.0f, -cur_camera_acceleration));
    if (keys['a'])
        camera->Accelerate(glm::vec3(-cur_camera_acceleration, 0.0f, 0.0f));
    if (keys['d']) 
        camera->Accelerate(glm::vec3(cur_camera_acceleration, 0.0f, 0.0f));
    if (keys['q'])
        camera->RotateLoc(ROLL_ANGLE, 0.0f, 0.0f, -1.0f);
    if (keys['e'])
        camera->RotateLoc(ROLL_ANGLE, 0.0f, 0.0f, 1.0f);
    keys.ClearNewKeys();
    camera->Update();
}

void App::onIdle()
{
    ProcessInput();
    //glutPostRedisplay();
}

void App::onReshape(int width, int height)
{
	this->window.width = width;
    this->window.height = height;
    glViewport(0, 0, width, height);
}

void App::onMouseMove(int x, int y)
{
    float MOUSE_SENSITIVITY = MOUSE_SENSITIVITY_PER_SEC * last_render_time;
    if (window.width / 2 == x && window.height / 2 == y)
        return;
    camera->RotateLoc(MOUSE_SENSITIVITY, window.height / 2 - y, window.width / 2 - x, 0.0f);
    glutWarpPointer(window.width / 2, window.height / 2);
}
