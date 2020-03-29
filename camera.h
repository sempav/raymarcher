#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "defines.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "logger.h"

class Camera {
    glm::vec3 pos, dir, up;

public:
    Camera(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 dir = glm::vec3(0.0f, 0.0f, 1.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));
    virtual ~Camera(void);

    glm::mat4 GetViewMatrix();
    void MoveLoc(float x, float y, float z, float distance = 1);
    void MoveGlob(float x, float y, float z, float distance = 1);
    void RotateLoc(float deg, float x, float y, float z);

    void SetPos(glm::vec3 pos) { this->pos = pos; }
    void SetDir(glm::vec3 dir) { this->dir = glm::normalize(dir); }
    void SetUp(glm::vec3 up) { this->up = glm::normalize(up); }
    void LookAt(glm::vec3 pos, glm::vec3 dir, glm::vec3 up) {
        SetPos(pos);
        SetDir(dir);
        SetUp(up);
    }

    glm::vec3 GetPos() const { return pos; }
    glm::vec3 GetDir() const { return dir; }
    glm::vec3 GetUp() const { return up; }
    glm::vec3 GetRight() const { return glm::cross(dir, up); }
};

class SmoothCamera : public Camera {
    glm::vec3 speed;

public:
    SmoothCamera(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 dir = glm::vec3(0.0f, 0.0f, 1.0f),
                 glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f))
        : Camera(pos, dir, up), speed(0) {}
    ~SmoothCamera(void) {}

    void Accelerate(glm::vec3 amount);
    void Update();
};
