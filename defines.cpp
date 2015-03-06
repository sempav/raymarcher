#include "defines.h"

const char *FILE_GEOM           = "data/geom.dat";
const char *FILE_LOG            = "log/cube.log";
const char *FILE_COMPILE_ERRORS = "log/errors.log";

const float CAMERA_ACCELERATION = 0.04;
const float CAMERA_SLOWDOWN = 0.9 * CAMERA_ACCELERATION;
const float CAMERA_MAX_SPEED = 0.14f;

const float STRAFE_SENSITIVITY_PER_SEC = 0.002f;
const float MOUSE_SENSITIVITY_PER_SEC  = 0.10f;
const float ROLL_ANGLE_PER_SEC         = 1.0f;

const int SCREEN_WIDTH  = 1000;
const int SCREEN_HEIGHT = 800;

float sgn(float a) {
	return (a > 0) ? 1 : 
	       (a < 0) ? -1 : 0;
}
