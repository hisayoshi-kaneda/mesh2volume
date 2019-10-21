#pragma once

#ifndef MESH_VOLUME_VIEWER
#define MESH_VOLUME_VIEWER

#include "FrameBufferObject.h"
#include "Shader.h"
#include "Texture2D.h"
#include "Texture2DArray.h"
#include "TriMesh.h"
#include "TriMeshLoader.h"
#include "VertexArrayObject.h"
#include "Volume.h"
#include "Window.h"
#include "common.h"

class Mesh2Volume : public Window {
private:
    int layerN = 30;
    VertexArrayObject vao;
    FrameBufferObject fbo;
    Shader mesh_shader;
    Shader depthPeeling_shader;
    Shader volumeGen_shader;
    Shader texture_shader;
    TriMesh *mesh;
    Texture2DArray colorImages;
    Texture2DArray LayeredDepthImages;

    string mesh_vert_file = "src/shaders/mesh_render.vert";
    string mesh_frag_file = "src/shaders/mesh_render.frag";
    string texture_vert_file = "src/shaders/texture_render.vert";
    string texture_frag_file = "src/shaders/texture_render.frag";
    string depthPeeling_vert_file = "src/shaders/depth_peeling.vert";
    string depthPeeling_frag_file = "src/shaders/depth_peeling.frag";
    string volumeGen_vert_file = "src/shaders/volumeGen.vert";
    string volumeGen_frag_file = "src/shaders/volumeGen.frag";

    glm::vec3 center;
    glm::vec3 size;
    float resolution;

public:
    Mesh2Volume(const int sizeX, const int sizeY, const int sizeZ, float resolution, TriMesh *mesh)
        : Window(sizeX, sizeY, "MeshViewer"),
          fbo(sizeX, sizeY),
          mesh(mesh),
          colorImages(sizeX, sizeY, layerN, GL_RGB32F, GL_RGBA),
          LayeredDepthImages(sizeX, sizeY, layerN, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT),
          size{sizeX, sizeY, sizeZ},
          resolution(resolution) {
        initialize();
    }

    virtual ~Mesh2Volume() {
    }

    void initialize() {
        vao.create(*mesh);
        mesh_shader.create(mesh_vert_file, mesh_frag_file);
        texture_shader.create(texture_vert_file, texture_frag_file);
        depthPeeling_shader.create(depthPeeling_vert_file, depthPeeling_frag_file);
        volumeGen_shader.create(volumeGen_vert_file, volumeGen_frag_file);
        mesh->computeGravity();
        center = mesh->gravity;
        projMat = glm::ortho(-size[0] * resolution / 2.0f, size[0] * resolution / 2.0f, -size[1] * resolution / 2.0f, size[1] * resolution / 2.0f, 0.1f, 150.0f);
        //projMat = glm::perspective(glm::radians(5.0f), (float)width / (float)height, 0.1f, 1000.0f);

        viewMat = glm::lookAt(center + glm::vec3(0.0f, 0.0f, 75.0f), center, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void main_loop() override {
        while (!glfwWindowShouldClose(window)) {
            generateLDI();
            //generateVolume();
            flush();
        }
    }

    void generateLDI() {
        fbo.bind();
        vao.bind();

        mesh_shader.bind();
        {
            mesh_shader.set_uniform_value(mvpMat(), "u_mvpMat");
            fbo.attachColorTexture(colorImages, 0);
            fbo.attachDepthTexture(LayeredDepthImages, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDrawElements(GL_TRIANGLES, mesh->verIndices.size(), GL_UNSIGNED_INT, 0);
        }
        mesh_shader.release();

        depthPeeling_shader.bind();
        {
            glm::mat4 normMat = glm::inverse(glm::transpose(mvMat()));
            normMat = mvMat();
            for (int i = 1; i < layerN; i++) {
                fbo.attachColorTexture(colorImages, i);
                fbo.attachDepthTexture(LayeredDepthImages, i);
                depthPeeling_shader.set_uniform_value(normMat, "u_normMat");
                depthPeeling_shader.set_uniform_value(mvpMat(), "u_mvpMat");
                depthPeeling_shader.set_uniform_value(!(i & 1), "is_front");
                depthPeeling_shader.set_uniform_value(i - 1, "layer");
                depthPeeling_shader.set_uniform_texture(LayeredDepthImages, "texImage");
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glDrawElements(GL_TRIANGLES, mesh->verIndices.size(), GL_UNSIGNED_INT, 0);
            }
        }
        depthPeeling_shader.release();

        fbo.release();
        vao.release();
    }

    Volume generateVolume() {
        Volume volume((int)size[0], (int)size[1], (int)size[2], resolution, 0.5f);
        generateLDI();

        fbo.bind();
        vao.bind();

        Texture2D colorBuffer(size[0], size[1], GL_RGB32F, GL_RGBA);
        Texture2D depthBuffer(size[0], size[1], GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT);
        fbo.attachColorTexture(colorBuffer);
        fbo.attachDepthTexture(depthBuffer);
        float coordZ = 0 * resolution;

        glm::mat4 mvpMat_ = mvpMat();

        float orgZ = -(size[2] - 1.0f) / 2.0f * resolution;

        volumeGen_shader.bind();
        for (int z = 0; z < size[2]; z++) {
            float coordZ = orgZ + z * resolution;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            volumeGen_shader.set_uniform_value(mvpMat_, "u_mvpMat");
            volumeGen_shader.set_uniform_value(glm::vec2(size), "size");
			volumeGen_shader.set_uniform_value(resolution, "resolution");
            volumeGen_shader.set_uniform_value(center, "center");
            volumeGen_shader.set_uniform_value(coordZ, "coordZ");
            volumeGen_shader.set_uniform_value(layerN, "layerN");
            volumeGen_shader.set_uniform_texture(LayeredDepthImages, "LayeredDepthImages");
            glDrawElements(GL_TRIANGLES, mesh->verIndices.size(), GL_UNSIGNED_INT, 0);
            glReadBuffer(GL_COLOR_ATTACHMENT0);
            glReadPixels(0, 0, size[0], size[1], GL_RED, GL_FLOAT, &volume.data[volume.size[0] * volume.size[1] * z]);
        }
        volumeGen_shader.release();

        vao.release();
        fbo.release();

        return volume;
    }
};

#endif //MESH_VIEWER