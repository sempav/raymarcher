#pragma once

#include "buffer.h"
#include "logger.h"
#include "shader.h"

class Mesh
{
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
	std::vector<GLuint>  indices;

	GLBuffer vbo_vertices;
	GLBuffer vbo_normals;
	GLBuffer ibo_indices;

	bool vbo_loaded;
public:
	Mesh(void);
	virtual ~Mesh(void);

	virtual bool LoadVBO();
	virtual void Draw(GLProgram *program);

	void SetVertices(GLfloat *vertices, int size);
	void SetNormals (GLfloat *normals,  int size);
	void SetIndices (GLuint  *indices,  int size);

	void FreeVertices();
	void FreeNormals();
	void FreeIndices();
};
