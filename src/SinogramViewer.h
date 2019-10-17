#pragma once

#ifndef SINOGRAMVIEWER_H
#define SINOGRAMVIEWER_H

#include "FrameBufferObject.h"
#include "Shader.h"
#include "VertexArrayObject.h"
#include "Window.h"
#include "common.h"

class SinogramViewer : public Window {
private:
	VertexArrayObject vao;
	Shader sinogram_shader;
	Shader texture_shader;
	FrameBufferObject fbo;
	TriMesh* mesh = nullptr;
	string sinogram_vert_file = "src/shaders/sinogram_render.vert";
	string sinogram_frag_file = "src/shaders/sinogram_render.frag";
	string texture_vert_file = "src/shaders/texture_render.vert";
	string texture_frag_file = "src/shaders/texture_render.frag";
	Texture2D textureBuffer;
	Texture2D depthBuffer;


public:
	SinogramViewer(int width, int height, TriMesh* mesh)
		: Window(width, height, "SinogramViewer"),
		fbo(width, height),
		mesh(mesh),
		textureBuffer(width, height,GL_RGB32F,GL_RGBA),
		depthBuffer(width,height,GL_DEPTH_COMPONENT,GL_DEPTH_COMPONENT){
		fbo.attachColorTexture(textureBuffer);
		fbo.attachDepthTexture(depthBuffer);
		initialize();
	}

	virtual ~SinogramViewer() {
	}

	void initialize() {
		vao.create(*mesh);
		sinogram_shader.create(sinogram_vert_file, sinogram_frag_file);
		texture_shader.create(texture_vert_file, texture_frag_file);
		mesh->computeGravity();
		gravity = mesh->gravity;
		rotCenter = gravity;
		viewMat = glm::lookAt(gravity + glm::vec3(20.0f, 0.0f, 0.0f), // ���_�̈ʒu
			gravity,
			glm::vec3(0.0f, 1.0f, 0.0f));
		//viewMat = glm::lookAt(glm::vec3(9.0f, 12.0f, 15.0f),  // ���_�̈ʒu
		//	glm::vec3(0.0f, 0.0f, 0.0f),  // ���Ă����
		//	glm::vec3(0.0f, 1.0f, 0.0f)); // ���E�̏����
	}

	float theta = 0.0f;

	void draw() override {
		// FBO�ւ̕`��
		sinogram_shader.bind();
		{
			fbo.bind();
			vao.bind();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Uniform�ϐ��̓]��
			const glm::mat4 normMat = glm::inverse(glm::transpose(mvMat()));
			sinogram_shader.set_uniform_value(mvpMat(), "u_mvpMat");
			sinogram_shader.set_uniform_value(mvMat(), "u_mvMat");
			sinogram_shader.set_uniform_value(normMat, "u_normMat");

			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);

			glDrawElements(GL_TRIANGLES, mesh->verIndices.size(), GL_UNSIGNED_INT, 0);

			glEnable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);

			fbo.release();
			vao.release();
		}
		sinogram_shader.release();

		// �E�B���h�E�ւ̕`��
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		texture_shader.bind();
		glBindTexture(GL_TEXTURE_2D, textureBuffer.textureId);

		glUniform1i(glGetUniformLocation(texture_shader.program_id, "texImage"), 0);

		vao.bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		vao.release();

		texture_shader.release();
	}

	void main_loop() override {
		while (!glfwWindowShouldClose(window)) {
			draw();
			flush();
		}
	}
};

#endif //VOLUMEVIEWER_H