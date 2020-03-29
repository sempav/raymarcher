#include "camera.h"

Camera::Camera(glm::vec3 pos, glm::vec3 dir, glm::vec3 up) {
    SetPos(pos);
    SetDir(dir);
    SetUp(up);
}

Camera::~Camera() {}

glm::mat4 Camera::GetViewMatrix() { return glm::lookAt(pos, pos + dir, up); }

void Camera::MoveLoc(float x, float y, float z, float distance) {
    glm::vec3 right = GetRight();
    SetPos(pos + distance * (x * right + y * up + z * dir));
}

void Camera::MoveGlob(float x, float y, float z, float distance) {
    SetPos(pos + distance * glm::vec3(x, y, z));
}

void Camera::RotateLoc(float deg, float x, float y, float z) {
    glm::vec3 right = GetRight();
    glm::vec3 new_dir = glm::rotate(dir, x * deg, right);
    new_dir = glm::rotate(new_dir, y * deg, up);
    glm::vec3 new_up = glm::rotate(up, x * deg, right);
    new_up = glm::rotate(new_up, z * deg, dir);
    SetDir(new_dir);
    SetUp(new_up);
}

void SmoothCamera::Accelerate(glm::vec3 amount) { speed += amount; }

void SmoothCamera::Update() {
    MoveLoc(speed.x, speed.y, speed.z, 1);
    for (int i = 0; i < 3; i++)
        if (speed[i] > 0)
            speed[i] = speed[i] - CAMERA_SLOWDOWN < 0 ? 0 : speed[i] - CAMERA_SLOWDOWN;
        else
            speed[i] = speed[i] + CAMERA_SLOWDOWN > 0 ? 0 : speed[i] + CAMERA_SLOWDOWN;
}
