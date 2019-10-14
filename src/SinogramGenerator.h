#pragma once

#ifndef SINOGRAMGEN_H
#define SINOGRAMGEN_H

#include "FrameBufferObject.h"
#include "Shader.h"
#include "Timer.h"
#include "VertexArrayObject.h"
#include "Window.h"
#include "common.h"

class SinogramGenerator : public Window {
private:
    VertexArrayObject vao;
    Shader sinogram_shader;
    FrameBufferObject fbo;
    TriMesh *mesh = nullptr;
    string sinogram_vert_file = "src/shaders/sinogram_render.vert";
    string sinogram_frag_file = "src/shaders/sinogram_render.frag";

public:
    SinogramGenerator(int width, int height, TriMesh *mesh) : Window(width, height, "SinogramViewer"),
                                                              fbo(width, height),
                                                              mesh(mesh) {
        initialize();
    }

    virtual ~SinogramGenerator() {
    }

    void initialize() {
        vao.create(*mesh);
        sinogram_shader.create(sinogram_vert_file, sinogram_frag_file);
        mesh->computeGravity();
        gravity = mesh->gravity;
        rotCenter = gravity;
        viewMat = glm::lookAt(gravity + glm::vec3(20.0f, 0.0f, 0.0f), // ���_�̈ʒu
                              gravity,                                // ���Ă����
                              glm::vec3(0.0f, 1.0f, 0.0f));           // ���E�̏����
    }

    float theta = 0.0f;

    void generate(int sino_num) {
        Timer timer;
        timer.start();

        vector<float *> sino(sino_num);
        for (int i = 0; i < sino_num; i++) {
            sino[i] = new float[width * height];
        }
        float theta = 2.0f * M_PI / sino_num;

        sinogram_shader.bind();
        vao.bind();
        for (int i = 0; i < sino_num; i++) {
            fbo.bind();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            const glm::mat4 normMat = glm::inverse(glm::transpose(mvMat()));
            sinogram_shader.set_uniform_value(mvpMat(), "u_mvpMat");
            sinogram_shader.set_uniform_value(mvMat(), "u_mvMat");
            sinogram_shader.set_uniform_value(normMat, "u_normMat");

            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);

            glDrawElements(GL_TRIANGLES, mesh->verIndices.size(), GL_UNSIGNED_INT, 0);

            glReadBuffer(GL_COLOR_ATTACHMENT0);
            glReadPixels(0, 0, width, height, GL_RED, GL_FLOAT, sino[i]);

            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);

            fbo.release();
        }
        vao.release();
        sinogram_shader.release();
        cout << timer.stop() << endl;
    }
};

#endif //VOLUMEVIEWER_H