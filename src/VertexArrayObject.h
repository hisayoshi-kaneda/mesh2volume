#pragma once

#ifndef VAO_H
#define VAO_H

#include "TriMesh.h"
#include "common.h"

// ���_�I�u�W�F�N�g
struct Vertex {
    Vertex(const glm::vec3 &position_, const glm::vec3 &color_)
        : position(position_), color(color_) {
    }

    glm::vec3 position;
    glm::vec3 color;
};

class VertexArrayObject {
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    GLuint vao_id = 0;

public:
    void add_vertex(const glm::vec3 &position, const glm::vec3 &color, const unsigned int &index) {
        vertices.push_back(Vertex(position, color));
        indices.push_back(index);
    }

    void add_vertex(const Vertex &v, const unsigned int &index) {
        vertices.push_back(v);
        indices.push_back(index);
    }

    void create() {
        GLuint vertex_buffer_id = 0;
        GLuint index_buffer_id = 0;
        // VAO�̍쐬
        glGenVertexArrays(1, &vao_id);
        glBindVertexArray(vao_id);

        // ���_�o�b�t�@�̍쐬
        glGenBuffers(1, &vertex_buffer_id);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        // ���_�o�b�t�@�̗L����
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color));

        // ���_�ԍ��o�b�t�@�̍쐬
        glGenBuffers(1, &index_buffer_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
                     indices.data(), GL_STATIC_DRAW);

        // VAO��OFF�ɂ��Ă���
        glBindVertexArray(0);
    }

    //For Mesh(Only position)
    void create(TriMesh &mesh) {
        mesh.computeVerNormals();
        // VAO�̍쐬
        glGenVertexArrays(1, &vao_id);
        glBindVertexArray(vao_id);

        GLuint buffer_id[2] = {0, 1};
        GLuint index_buffer_id = 0;

        // ���_�o�b�t�@�̍쐬
        glGenBuffers(2, buffer_id);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

        glBindBuffer(GL_ARRAY_BUFFER, buffer_id[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.verNormals.size(), mesh.verNormals.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
        // ���_�ԍ��o�b�t�@�̍쐬
        glGenBuffers(1, &index_buffer_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.verIndices.size(),
                     mesh.verIndices.data(), GL_STATIC_DRAW);

        // VAO��OFF�ɂ��Ă���
        glBindVertexArray(0);
    }

    void bind() {
        glBindVertexArray(vao_id);
    }

    void release() {
        glBindVertexArray(0);
    }
};

#endif //VAO_H