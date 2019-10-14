#pragma once

#ifndef MESH_VIEWER
#define MESH_VIEWER

#include "FrameBufferObject.h"
#include "Shader.h"
#include "TriMesh.h"
#include "TriMeshLoader.h"
#include "VertexArrayObject.h"
#include "Window.h"
#include "common.h"

class MeshViewer : public Window {
private:
    VertexArrayObject vao;
    FrameBufferObject fbo;
    Shader mesh_shader;
    Shader texture_shader;
    TriMesh *mesh;
    string mesh_vert_file = "src/shaders/mesh_render.vert";
    string mesh_frag_file = "src/shaders/mesh_render.frag";
    string texture_vert_file = "src/shaders/texture_render.vert";
    string texture_frag_file = "src/shaders/texture_render.frag";
    glm::vec3 cameraPos;
    glm::vec3 upVector;
    glm::vec3 cameraTarget;

public:
    MeshViewer(const int width, const int height, TriMesh *mesh)
        : Window(width, height, "MeshViewer"),
          mesh(mesh),
          fbo(width, height) {
        initialize();
    }

    virtual ~MeshViewer() {
    }

    void initialize() {
        vao.create(*mesh);
        mesh_shader.create(mesh_vert_file, mesh_frag_file);
        texture_shader.create(texture_vert_file, texture_frag_file);
        mesh->computeGravity();
        gravity = mesh->gravity;
        rotCenter = gravity;
        cameraPos = gravity + glm::vec3(1.0f, 0.0f, 0.0f);
        cameraTarget = gravity;
        upVector = glm::vec3(0.0f, 1.0f, 0.0f);
        viewMat = glm::lookAt(cameraPos,    // ���_�̈ʒu
                              cameraTarget, // ���Ă����
                              upVector);    // ���E�̏����
    }

    void draw() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        texture_shader.bind();

        glUniform1f(glGetUniformLocation(texture_shader.program_id, "sinogram"), fbo.textureBufferIds[0]);

        vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        vao.release();

        texture_shader.release();
    }

    void main_loop() override {
        generateLDI();
        dump(fbo.textureBufferIds);
        dump(fbo.colorAttachmentIds);
        while (!glfwWindowShouldClose(window)) {
            draw();
            flush();
        }
    }

private:
    void generateLDI() {
        glEnable(GL_ALPHA_TEST);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* 書き込むポリゴンのテクスチャ座標値のＲとテクスチャとの比較を行うようにする */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

        /* もしＲの値がテクスチャの値を超えていたら真（フラグメントを描く） */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_GREATER);

        /* 比較の結果をアルファ値として得る */
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_ALPHA);

        /* アルファテストの比較関数（しきい値） */
        glAlphaFunc(GL_GEQUAL, 0.5f);

        mesh_shader.bind();
        vao.bind();
        fbo.bind();

        mesh_shader.set_uniform_value(mvpMat(), "u_mvpMat");

        int layerN = 10;
        fbo.setRenderTarget();
        glDrawElements(GL_TRIANGLES, mesh->verIndices.size(), GL_UNSIGNED_INT, 0);

        for (int i = 1; i < layerN; i++) {
            fbo.addColorAttachment();
            fbo.setRenderTarget();
            fbo.bindTexture(i - 1);
            glDrawElements(GL_TRIANGLES, mesh->verIndices.size(), GL_UNSIGNED_INT, 0);
            fbo.releaseTexture();
        }
        vao.release();
        mesh_shader.release();
        fbo.release();

        glDisable(GL_ALPHA_TEST);
    }
};

#endif //MESH_VIEWER