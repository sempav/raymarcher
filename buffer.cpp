#include "buffer.h"

GLBuffer::GLBuffer(GLenum target) : name(0), target(target)
{
}

GLBuffer::~GLBuffer(void)
{
    if (!Empty()) glDeleteBuffers(1, &name);
}

void GLBuffer::Generate()
{
    glGenBuffers(1, &name);
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        logger.Write("Failed to generate buffer.\n");
        logger.LogOpenGLError(error);
    }
}

void GLBuffer::Bind()
{
    if (Empty()) {
        logger.Write("Error: cannot bind empty buffer.\n");
        return;
    }
    glBindBuffer(target, name);
}

void GLBuffer::Unbind()
{
    glBindBuffer(target, 0);
}

bool GLBuffer::BufferData(GLsizeiptr size, const GLvoid *data, GLenum usage)
{
    Bind();
    if (!name) return false;
    glBufferData(target, size, data, usage);
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        logger.Write("Failed to set buffer data.\n");
        logger.LogOpenGLError(error);
        return false;
    }
    else return true;
}

void GLBuffer::Delete()
{
    glDeleteBuffers(1, &name);
    name = 0;
}

void GLBuffer::SetTarget(GLenum target) 
{
    logger.Write("Why the hell would you ever change buffer's target?\n");
    if (name) {
        logger.Write("Error: changing target of a non-empty buffer.\n");
        Delete();
        Generate();
    }
    this->target = target;
}

int GLBuffer::GetSize()
{
    int size;
    glGetBufferParameteriv(target, GL_BUFFER_SIZE, &size);
    return size;
}
