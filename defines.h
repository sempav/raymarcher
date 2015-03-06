#pragma once

#define ALLOW_FULLSCREEN 0
#define SLEEP_TIME 0

#include <cassert>
#include <ctime>
#include <cctype>
#include <math.h>
#include <string>
#include <stdio.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtx/rotate_vector.hpp>

#include <iterator>
#include <list>
#include <vector>

extern const char *FILE_GEOM;
extern const char *FILE_LOG;
extern const char *FILE_COMPILE_ERRORS;


extern const float CAMERA_ACCELERATION;
extern const float CAMERA_SLOWDOWN;
extern const float CAMERA_MAX_SPEED;

extern const float STRAFE_SENSITIVITY_PER_SEC;
extern const float MOUSE_SENSITIVITY_PER_SEC;
extern const float ROLL_ANGLE_PER_SEC;

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

float sgn(float a);
