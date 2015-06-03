#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtx/rotate_vector.hpp>

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

extern const int FRAMERATE;

float sgn(float a);
