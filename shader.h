#pragma once

#include <memory>

#include "camera.h"
#include "defines.h"
#include "logger.h"

class GLShader {
protected:
    GLuint shader;
    GLenum type;

public:
    GLShader(GLenum type);
    GLShader(const char *filename, GLenum type);
    ~GLShader(void);

    bool LoadFromFile(const char *filename);
    bool Load(const char *source);

    GLuint GetId() const { return shader; }
};

class GLProgram {
protected:
    GLuint program;
    bool active;
    int cnt_lights;

    std::unique_ptr<GLShader> vertex;
    std::unique_ptr<GLShader> fragment;

    GLProgram(const GLProgram &a) = delete;
    GLProgram &operator=(const GLProgram &a) = delete;

public:
    GLProgram(void);
    ~GLProgram(void);

    bool LoadVertexShader(GLShader *vertex);
    bool LoadFragmentShader(GLShader *fragment);

    void UnloadVertexShader() { vertex.reset(); }
    void UnloadFragmentShader() { fragment.reset(); }

    bool Link();
    void Use();

    bool GetAttribute(GLint *attribute, const char *name);
    bool GetUniform(GLint *uniform, const char *name);
    GLint GetAttribute(const char *name);
    GLint GetUniform(const char *name);

    bool SetVertexAttrib(const char *name, GLint size, GLenum type, GLboolean normalized,
                         GLsizei stride, const GLvoid *pointer);
    bool DisableVertexAttrib(const char *name);
    void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);

    bool SetUniform1f(const char *name, GLfloat value);
    bool SetUniform3fv(const char *name, GLsizei count, const GLfloat *value);
    bool SetUniform4fv(const char *name, GLsizei count, const GLfloat *value);
    bool SetUniformMat2(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value);
    bool SetUniformMat3(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value);
    bool SetUniformMat4(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value);

    void SetCamera(const Camera &camera, float aspect_ratio);
};
