#include "mesh.h"

Mesh::Mesh(void)
    : vertices(),
      normals(),
      indices(),
      vbo_vertices(GL_ARRAY_BUFFER),
      vbo_normals(GL_ARRAY_BUFFER),
      ibo_indices(GL_ELEMENT_ARRAY_BUFFER),
      vbo_loaded(false) {}

Mesh::~Mesh(void) {
    if (!vbo_vertices.Empty()) vbo_vertices.Delete();
    if (!vbo_normals.Empty()) vbo_normals.Delete();
    if (!ibo_indices.Empty()) ibo_indices.Delete();
}

bool Mesh::LoadVBO() {
    if (vertices.empty() || indices.empty()) {
        logger.Write("Error loading VBO: vertex data not assigned.\n");
        return false;
    }

    if (normals.empty()) {
        logger.Write("Warning: loading mesh without normals.\n");
    }

    logger.Write("vbo_vertices\n");
    vbo_vertices.Generate();
    if (!vbo_vertices.BufferData(vertices.size() * sizeof(GLfloat), vertices.data(),
                                 GL_STATIC_DRAW)) {
        logger.Write("Can't load vertex buffer.\n");
        vbo_vertices.Delete();
        return false;
    }
    vbo_vertices.Unbind();

    if (!normals.empty()) {
        logger.Write("vbo_normals\n");
        vbo_normals.Generate();
        if (!vbo_normals.BufferData(normals.size() * sizeof(GLfloat), normals.data(),
                                    GL_STATIC_DRAW)) {
            logger.Write("Can't load normal buffer.\n");
            vbo_normals.Delete();
            return false;
        }
        vbo_normals.Unbind();
    }

    logger.Write("ibo_indices\n");
    ibo_indices.Generate();
    if (!ibo_indices.BufferData(indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW)) {
        logger.Write("Can't load index buffer.\n");
        ibo_indices.Delete();
        return false;
    }
    ibo_indices.Unbind();

    vbo_loaded = true;
    return true;
}

void Mesh::Draw(GLProgram *program) {
    if (!vbo_loaded) {
        logger.Write("Error drawing mesh: VBO not loaded.\n");
        return;
    }

    program->Use();

    vbo_vertices.Bind();
    program->SetVertexAttrib("coord", 3, GL_FLOAT, GL_FALSE, 0, 0);
    vbo_vertices.Unbind();

    if (!vbo_normals.Empty()) {
        vbo_normals.Bind();
        program->SetVertexAttrib("normal", 3, GL_FLOAT, GL_FALSE, 0, 0);
        vbo_normals.Unbind();
    }

    ibo_indices.Bind();
    program->DrawElements(GL_TRIANGLES, ibo_indices.GetSize() / sizeof(GLuint), GL_UNSIGNED_INT, 0);
    ibo_indices.Unbind();

    program->DisableVertexAttrib("coord");
    if (!vbo_normals.Empty()) program->DisableVertexAttrib("normal");
}

void Mesh::SetVertices(GLfloat *vertices, int size) {
    logger.Write("assigning vertices..");
    if (!this->vertices.empty()) {
        logger.Write("Error: overwriting mesh vertices.\n");
        return;
    }
    this->vertices.assign(vertices, vertices + size);
    logger.Write("OK\n");
    // logger.Write("vertices:\n");
    // logger.LogVector(this->vertices);
}

void Mesh::SetNormals(GLfloat *normals, int size) {
    logger.Write("assigning normals..");
    if (!this->normals.empty()) {
        logger.Write("Error: overwriting mesh normals.\n");
        return;
    }
    this->normals.assign(normals, normals + size);
    logger.Write("OK\n");
    // logger.Write("normals:\n");
    // logger.LogVector(this->normals);
}

void Mesh::SetIndices(GLuint *indices, int size) {
    logger.Write("assigning indices..");
    if (!this->indices.empty()) {
        logger.Write("Error: overwriting mesh indices.\n");
        return;
    }
    this->indices.assign(indices, indices + size);
    logger.Write("OK\n");
    // logger.Write("indices:\n");
    // logger.LogVector(this->indices);
}

void Mesh::FreeVertices() {
    if (!vertices.empty()) {
        vertices.clear();
    } else
        logger.Write("Warning: trying to free an empty vertex array.\n");
}

void Mesh::FreeIndices() {
    if (!indices.empty()) {
        indices.clear();
    } else
        logger.Write("Warning: trying to free an empty index array.\n");
}
