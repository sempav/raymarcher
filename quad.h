#pragma once

#include "mesh.h"

class Quad
{
    Mesh mesh;
    
public:
    Quad(): mesh() { }
    bool LoadVBO();
    void Draw(GLProgram* program);
};
