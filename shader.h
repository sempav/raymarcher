#pragma once

#include "defines.h"
#include "logger.h"

#include "camera.h"

class GLShader
{
protected:
    GLuint shader;
    GLenum type;
    bool active;
public:
    GLShader(GLenum type);
    GLShader(const char *filename, GLenum type);
    ~GLShader(void);

    bool LoadFromFile(const char *filename);
    bool Load(const char *source);

    GLuint GetId()  { return shader; }
    bool IsActive() { return active; }
};

class GLProgram
{
protected:
    GLuint program;
    bool active;
    int cnt_lights;

    GLShader *vertex;
    GLShader *fragment;

    GLProgram(const GLProgram &a) = delete;
    GLProgram &operator= (const GLProgram &a) = delete;
public:
    GLProgram(void);
    ~GLProgram(void);

    bool LoadVertexShader(const char *filename);
    bool LoadFragmentShader(const char *filename);
    void UnloadVertexShader();
    void UnloadFragmentShader();

    bool Link();
    void Use();

    bool GetAttribute(GLint *attribute, const char *name);
    bool GetUniform(GLint *uniform, const char *name);
    GLint GetAttribute(const char *name);
    GLint GetUniform(const char *name);

    bool SetVertexAttrib(const char *name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
    bool DisableVertexAttrib(const char *name);
    void DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);

    bool SetUniform1f(const char *name, GLfloat value);
    bool SetUniform3fv(const char *name, GLsizei count, const GLfloat *value);
    bool SetUniform4fv(const char *name, GLsizei count, const GLfloat *value);
    bool SetUniformMat2(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value);
    bool SetUniformMat3(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value);
    bool SetUniformMat4(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value);

    void SetCamera(const Camera &camera);

    GLShader* GetVertexShader()   const { return vertex;   }
    GLShader* GetFragmentShader() const { return fragment; }
    bool IsActive()               const { return active;   }
};
