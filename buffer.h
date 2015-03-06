#pragma once

#include "defines.h"
#include "logger.h"

class GLBuffer
{
	GLuint name;
	GLenum target;

public:
	GLBuffer(GLenum target);
	virtual ~GLBuffer(void);

	void Generate();
	void Bind();
	void Unbind();
	bool BufferData(GLsizeiptr size, const GLvoid *data, GLenum usage);
	void Delete();

	bool Empty() { return 0 == name; }

	int GetSize();

	void SetTarget(GLenum target);
};
