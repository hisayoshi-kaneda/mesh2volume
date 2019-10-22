#pragma once

#ifndef MESH_VOLUME_VIEWER
#define MESH_VOLUME_VIEWER

#include "FrameBufferObject.h"
#include "Shader.h"
#include "Texture2D.h"
#include "Texture2DArray.h"
#include "Timer.h"
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
    shared_ptr<TriMesh> mesh;
    Texture2DArray colorImages;
    Texture2DArray LayeredDepthImagesX;
    Texture2DArray LayeredDepthImagesY;
    Texture2DArray LayeredDepthImagesZ;

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
    float zFar;
    float zNear;

public:
    Mesh2Volume(const int sizeX, const int sizeY, const int sizeZ, float resolution, shared_ptr<TriMesh> mesh)
        : Window(sizeX, sizeY, "MeshViewer"),
          fbo(sizeX, sizeY),
          mesh(mesh),
          colorImages(sizeX, sizeY, layerN, GL_RGB32F, GL_RGBA),
          LayeredDepthImagesX(sizeY, sizeZ, layerN, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_TEXTURE0),
          LayeredDepthImagesY(sizeZ, sizeX, layerN, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_TEXTURE1),
          LayeredDepthImagesZ(sizeX, sizeY, layerN, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_TEXTURE2),
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
        mesh->computeAABB();
        center = mesh->centerAABB;
        zFar = max(size[0], max(size[1], size[2])) * resolution + 10.0f;
		zNear = 0.1f;
    }

	void draw()override {
		generateLDI(LayeredDepthImagesZ, glm::vec2(size[0], size[1]), glm::vec3(0.0f, 0.0f, resolution * size[2] / 2.0f + 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		generateVolume();
	}

    void generateLDI(Texture2DArray &LayeredDepthImages, glm::vec2 size, glm::vec3 cameraPos, glm::vec3 upVector) {
		projMat = glm::ortho(-size[0] * resolution / 2.0f, size[0] * resolution / 2.0f, -size[1] * resolution / 2.0f, size[1] * resolution / 2.0f, zNear, zFar);
        viewMat = glm::lookAt(cameraPos, glm::vec3(0.0f), upVector);
		modelMat = glm::translate(-center);

        fbo.bind();
        vao.bind();

		glViewport(0.0f,0.0f,int(size.x),int(size.y));
        mesh_shader.bind();
        {
            mesh_shader.set_uniform_value(mvpMat(), "u_mvpMat");
            fbo.attachDepthTexture(LayeredDepthImages, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDrawElements(GL_TRIANGLES, mesh->verIndices.size(), GL_UNSIGNED_INT, 0);
        }
        mesh_shader.release();

        depthPeeling_shader.bind();
        {
            for (int i = 1; i < layerN; i++) {
                fbo.attachDepthTexture(LayeredDepthImages, i);
                depthPeeling_shader.set_uniform_value(mvMat(), "u_mvMat");
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
		Timer timer;
		timer.start();

		Volume volume((int)size[0], (int)size[1], (int)size[2], resolution, 0.5f);
		float cameraPosX = resolution * size[0] / 2.0f + 1.0f;
		float cameraPosY = resolution * size[1] / 2.0f + 1.0f;
		generateLDI(LayeredDepthImagesX, glm::vec2(size[1], size[2]), glm::vec3(cameraPosX, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		generateLDI(LayeredDepthImagesY, glm::vec2(size[2], size[0]), glm::vec3(0.0f, cameraPosY, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		generateLDI(LayeredDepthImagesZ, glm::vec2(size[0], size[1]), glm::vec3(0.0f, 0.0f, resolution * size[2] / 2.0f + 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		projMat = glm::ortho(-size[0] * resolution / 2.0f, size[0] * resolution / 2.0f, -size[1] * resolution / 2.0f, size[1] * resolution / 2.0f, zNear, zFar);
		viewMat = glm::lookAt(glm::vec3(0.0f, 0.0f, resolution * size[2] / 2.0f + 1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMat = glm::mat4(1.0f);

		fbo.bind();
		vao.bind();

		Texture2D colorBuffer(size[0], size[1], GL_R32F, GL_RGBA);
		Texture2D depthBuffer(size[0], size[1], GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT);
		fbo.attachColorTexture(colorBuffer);
		fbo.attachDepthTexture(depthBuffer);

		glm::mat4 mvpMat_ = mvpMat();

		float orgZ = -(size[2] - 1.0f) / 2.0f * resolution;
		volumeGen_shader.bind();
		glViewport(0.0f, 0.0f, int(size[0]), int(size[1]));
        for (int z = 0; z < int(size[2]); z++) {
            float coordZ = orgZ + z * resolution;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            volumeGen_shader.set_uniform_value(mvpMat_, "u_mvpMat");
            volumeGen_shader.set_uniform_value(size, "size");
            volumeGen_shader.set_uniform_value(resolution, "resolution");
            volumeGen_shader.set_uniform_value(coordZ, "coordZ");
            volumeGen_shader.set_uniform_value(z, "indexZ");
            volumeGen_shader.set_uniform_value(layerN, "layerN");
            volumeGen_shader.set_uniform_value(zNear, "zNear");
            volumeGen_shader.set_uniform_value(zFar, "zFar");
            volumeGen_shader.set_uniform_value( cameraPosX, "cameraPosX");
            volumeGen_shader.set_uniform_value( cameraPosY, "cameraPosY");
            volumeGen_shader.set_uniform_texture(LayeredDepthImagesX, "LayeredDepthImagesX");
            volumeGen_shader.set_uniform_texture(LayeredDepthImagesY, "LayeredDepthImagesY");
            volumeGen_shader.set_uniform_texture(LayeredDepthImagesZ, "LayeredDepthImagesZ");
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glReadBuffer(GL_COLOR_ATTACHMENT0);
            glReadPixels(0, 0, size[0], size[1], GL_RED, GL_FLOAT, &volume.data[volume.size[0] * volume.size[1] * z]);
        }
        volumeGen_shader.release();

        vao.release();
        fbo.release();

        cout << "Generating volume took " << timer.stop() << " sec" << endl;

        return volume;
    }
};

#endif //MESH_VIEWER