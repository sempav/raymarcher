#include "shader.h"

GLShader::GLShader(GLenum type) 
{
    shader = 0;
    this->type = type;
}

GLShader::GLShader(const char *filename, GLenum type)
{
    this->type = type;
    LoadFromFile(filename);
}

GLShader::~GLShader(void) 
{
    // a value of 0 for shader will be silently ignored
    glDeleteShader(shader);
}

bool GLShader::LoadFromFile(const char *filename) 
{
    FILE* input = fopen(filename, "rb");
    if (!input || fseek(input, 0, SEEK_END) == -1) {
        if (input) fclose(input);
        return 0;
    }

    long size = ftell(input);
    if (size == -1 || fseek(input, 0, SEEK_SET) == -1) {
        fclose(input);
        return 0;
    }

    char *content = static_cast<char*>(malloc(size + 1)); 
    if (!content) {
        fclose(input);
        return 0;
    }

    fread(content, 1, size, input);
    if (ferror(input)) {
        free(content);
        fclose(input);
        return 0;
    }
    fclose(input);
    content[size] = '\0';
    bool ok = Load(content);
    free(content);
    return ok;
}

bool GLShader::Load(const char *source)
{
    if (shader) {
        throw std::runtime_error("tried to reload shader source");
    }

    if (source == nullptr) {
        logger.Write("Error: shader source not specified.\n");
        return 0;
    }

    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint compile_ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_ok);
    if (compile_ok == GL_FALSE) {
        logger.Write("Failed to compile shader.\n");
        logger.LogShaderInfo(shader);
        glDeleteShader(shader);
        throw std::runtime_error("failed to compile shader");
        return 0;
    }

    return 1;
}

GLProgram::GLProgram(void) : active(false), cnt_lights(0), vertex(), fragment()
{ }

GLProgram::~GLProgram(void)
{
    UnloadVertexShader();
    UnloadFragmentShader();
}

bool GLProgram::LoadVertexShader(GLShader *v)
{
    vertex.reset(v);
    return true;
}

bool GLProgram::LoadFragmentShader(GLShader *f)
{
    fragment.reset(f);
    return true;
}

bool GLProgram::Link()
{
    logger.Write("Linking program...\n");

    if (!vertex || !fragment) {
        if (!vertex)   logger.Write("Error: vertex shader not specified.\n");
        if (!fragment) logger.Write("Error: fragment shader not specified.\n");
        logger.Write("Aborting linking.\n");
        return 0;
    }

    program = glCreateProgram();
    glAttachShader(program, vertex->GetId());
    glAttachShader(program, fragment->GetId());

    glLinkProgram(program);

    // shader don't get deleted before they're detached
    // so it's best to detach them as soon as possible
    glDetachShader(program, vertex->GetId());
    glDetachShader(program, fragment->GetId());

    GLint link_ok = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
    if (link_ok == GL_FALSE) {
        logger.Write("Linking failed.\n");
        logger.LogShaderInfo(program);
        return 0;
    }
    logger.Write("Linked successfully.\n");

    return 1;
}

void GLProgram::Use()
{
    glUseProgram(program);
}

bool GLProgram::GetAttribute(GLint *attribute, const char *name)
{
    *attribute = glGetAttribLocation(program, name);
    if (*attribute == -1)
        logger.Write("Failed to bind attribute %s\n", name);        
    return (*attribute != -1);
}

bool GLProgram::GetUniform(GLint *uniform, const char *name)
{
    *uniform = glGetUniformLocation(program, name);
    if (*uniform == -1)
        logger.Write("Failed to bind uniform %s\n", name);
    return (*uniform != -1);
}

GLint GLProgram::GetAttribute(const char *name)
{
    GLint t = glGetAttribLocation(program, name);
    if (-1 == t)
        logger.Write("Failed to bind attribute %s\n", name);
    return t;
}

GLint GLProgram::GetUniform(const char *name)
{
    GLint t = glGetUniformLocation(program, name);
    if (-1 == t)
        logger.Write("Failed to bind uniform %s\n", name);
    return t;
}

bool GLProgram::SetVertexAttrib(const char *name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
{
    GLint t = GetAttribute(name);
    if (-1 == t) return false;
    glEnableVertexAttribArray(t);
    glVertexAttribPointer(t, size, type, normalized, stride, pointer);
    return true;
}

bool GLProgram::DisableVertexAttrib(const char *name)
{
    GLint t = GetAttribute(name);
    if (-1 == t) return false;
    glDisableVertexAttribArray(t);
    return true;
}

void GLProgram::DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
    glDrawElements(mode, count, type, indices);
}

bool GLProgram::SetUniform1f(const char *name, GLfloat value)
{
    GLint t;
    if (!GetUniform(&t, name)) return false;
    glUniform1f(t, value);
    return true;
}

bool GLProgram::SetUniform3fv(const char *name, GLsizei count, const GLfloat *value)
{
    GLint t;
    if (!GetUniform(&t, name)) return false;
    glUniform3fv(t, count, value);
    return true;
}

bool GLProgram::SetUniform4fv(const char *name, GLsizei count, const GLfloat *value)
{
    GLint t;
    if (!GetUniform(&t, name)) return false;
    glUniform4fv(t, count, value);
    return true;
}

bool GLProgram::SetUniformMat2(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GLint t;
    if (!GetUniform(&t, name)) return false;
    glUniformMatrix2fv(t, count, transpose, value);
    return true;
}

bool GLProgram::SetUniformMat3(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GLint t;
    if (!GetUniform(&t, name)) return false;
    glUniformMatrix3fv(t, count, transpose, value);
    return true;
}

bool GLProgram::SetUniformMat4(const char *name, GLsizei count, GLboolean transpose, const GLfloat *value)
{
    GLint t;
    if (!GetUniform(&t, name)) return false;
    glUniformMatrix4fv(t, count, transpose, value);
    return true;
}

void GLProgram::SetCamera(const Camera &camera, float aspect_ratio)
{
    SetUniform3fv("camera_pos"  , 1, glm::value_ptr(camera.GetPos()));
    SetUniform3fv("camera_dir"  , 1, glm::value_ptr(camera.GetDir()));
    SetUniform3fv("camera_up"   , 1, glm::value_ptr(camera.GetUp() ));
    SetUniform3fv("camera_right", 1, glm::value_ptr(camera.GetRight() * aspect_ratio));
    SetUniform1f ("camera_eye_dist", 3); // 60 degrees fovy
}
