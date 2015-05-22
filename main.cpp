#include "defines.h"
#include "logger.h"
#include "game.h"
#include "shader.h"

#include <iostream>

const float FPS_CAP = 60.0f;

static Game *game = NULL;

void onDisplay()
{
    game->onIdle();
    game->onDisplay();
}

void onIdle()
{
    game->onIdle();
}

void onReshape(int width, int height)
{
    game->onReshape(width, height);
}

void KeyboardFunc(unsigned char key, int x, int y)
{
    game->keys.PressKey(tolower(key));
}

void KeyboardUpFunc(unsigned char key, int x, int y)
{
    game->keys.ReleaseKey(tolower(key));
}

void MotionFunc(int x, int y)
{
    game->onMouseMove(x, y);
}

void TimerFunc(int)
{
    glutPostRedisplay();
    glutTimerFunc(1000/FPS_CAP, TimerFunc, 0);
}

void free_resources()
{
    if (game) delete game;
    logger.LogShutdown();
}

int main(int argc, char **argv)
{
    logger.LogStartup(argc, argv);

    game = new Game();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(game->GetWindowInfo()->width, game->GetWindowInfo()->height);
    GLuint idWindow = glutCreateWindow("Ima title");

    logger.LogSystemInfo();

    GLenum glew_status = glewInit();
    if (glew_status != GLEW_OK) {
        logger.Write("Error initializing GLEW: %s\n", glewGetErrorString(glew_status));
        return 0;
    }

    if (!GLEW_VERSION_2_0) {
        logger.Write("Error: OpenGL 2.0 not supported.\n");
        std::cerr << "Error: OpenGL 2.0 not supported.\n";
        //MessageBox(NULL, "OpenGL 2.0 not supported.", "Error", MB_ICONEXCLAMATION);
        return 0;
    }

    if (game->Initialize()) {
        atexit(free_resources);

        glutWarpPointer(game->GetWindowInfo()->width / 2, game->GetWindowInfo()->height / 2);
        glutSetCursor(GLUT_CURSOR_NONE);

        glutDisplayFunc(onDisplay);
        //glutIdleFunc(onIdle);
        glutTimerFunc(1000/FPS_CAP, TimerFunc, 0);
        glutReshapeFunc(onReshape);
        glutKeyboardFunc(KeyboardFunc);
        glutKeyboardUpFunc(KeyboardUpFunc);
        glutMotionFunc(MotionFunc);
        glutPassiveMotionFunc(MotionFunc);

        glutMainLoop();
    }
    
    delete game;
    game = NULL;
    glutDestroyWindow(idWindow);
    return 0;
}
