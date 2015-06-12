#include "defines.h"

const char *FILE_LOG            = "log/main.log";
const char *FILE_COMPILE_ERRORS = "log/errors.log";

const float CAMERA_ACCELERATION = 0.04;
const float CAMERA_SLOWDOWN = 0.9 * CAMERA_ACCELERATION;
const float CAMERA_MAX_SPEED = 0.14f;

const float STRAFE_SENSITIVITY_PER_SEC = 0.002f;
const float MOUSE_SENSITIVITY_PER_SEC  = 0.05f;
const float ROLL_ANGLE_PER_SEC         = 1.0f;

const int SCREEN_WIDTH  = 1000;
const int SCREEN_HEIGHT = 800;

const int FRAMERATE = 60;
const int MSEC_PER_FRAME = 1000.0 / FRAMERATE;

float sgn(float a) {
	return (a > 0) ? 1 : 
	       (a < 0) ? -1 : 0;
}
