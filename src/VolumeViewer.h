#pragma once

#ifndef VOLUMEVIEWER_H
#define VOLUMEVIEWER_H

#include "Shader.h"
#include "Texture3D.h"
#include "VertexArrayObject.h"
#include "Volume.h"
#include "Window.h"
#include "common.h"

class VolumeViewer : public Window {
private:
    VertexArrayObject vao;
    Shader volume_shader;
    Shader shader_sinogram;
    Volume *volume = nullptr;
    Texture3D texture;
    string vol_vert_file = "src/shaders/volume_render.vert";
    string vol_frag_file = "src/shaders/volume_render.frag";
    string sino_vert_file = "src/shaders/sinogram_render.vert";
    string sino_frag_file = "src/shaders/sinogram_render.frag";
    int is_volumeMode = 1;

public:
    VolumeViewer(int width, int height, Volume *vol) : Window(width, height, "VolumeViewer"),
                                                       volume(vol),texture(*vol) {
        initialize();
    }

    virtual ~VolumeViewer() {
    }

    void initialize() {
        vao.create();
        volume_shader.create(vol_vert_file, vol_frag_file);
        shader_sinogram.create(sino_vert_file, sino_frag_file);

		// カメラの初期化
		projMat = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
		viewMat = glm::lookAt(glm::vec3(0.0f, 0.0f, 50.0f),  // 視点の位置
			glm::vec3(0.0f, 0.0f, 0.0f),  // 見ている先
			glm::vec3(0.0f, 1.0f, 0.0f)); // 視界の上方向

		//その他の行列の初期化
		modelMat = glm::mat4(1.0);
    }

    void draw() override {
        glm::vec3 volume_size = glm::vec3(volume->size[0], volume->size[1], volume->size[2]);
		//バインド
        volume_shader.bind();
        vao.bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Uniform変数の転送
        volume_shader.set_uniform_value(projMat, "u_projMat");
        volume_shader.set_uniform_value(mvMat(), "u_mvMat");
        volume_shader.set_uniform_value(volume_size, "u_volumeSize");
        volume_shader.set_uniform_value(volume->resolution, "u_volumeResolution");
        volume_shader.set_uniform_value(volume->threshold, "u_threshold");
        volume_shader.set_uniform_texture(texture, "u_volumeTexture");

        glDrawArrays(GL_TRIANGLES, 0, 6);

        //アンバインド
        vao.release();
        volume_shader.release();
        texture.release();
    }

	void main_loop() override {
		while (!glfwWindowShouldClose(window)) {
			draw();
			flush();
		}
	}
};

#endif //VOLUMEVIEWER_H