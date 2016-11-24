#include "quad.h"

bool Quad::LoadVBO()
{
    logger.Write("Loading quad VBO.\n");
    GLfloat vertices[12] = {
        -1.0, -1.0, 0.0,
         1.0, -1.0, 0.0,
         1.0,  1.0, 0.0,
        -1.0,  1.0, 0.0
    };
    GLuint indices[6] = {
        0, 1, 2,
        2, 3, 0
    };
    mesh.SetVertices(vertices, 12);
    mesh.SetIndices(indices, 6);
    return mesh.LoadVBO();
}

void Quad::Draw(GLProgram *program)
{
    mesh.Draw(program);
}
