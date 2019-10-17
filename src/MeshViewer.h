#pragma once

#ifndef MESH_VIEWER
#define MESH_VIEWER

#include "common.h"
#include "VertexArrayObject.h"
#include "Shader.h"
#include "Window.h"
#include "TriMesh.h"
#include "TriMeshLoader.h"

class MeshViewer :public Window {
private:
	VertexArrayObject vao;
	Shader shader;
	TriMesh* mesh;
	string vert_file = "src/shaders/mesh_render.vert";
	string frag_file = "src/shaders/mesh_render.frag";
	glm::vec3 cameraPos;
	glm::vec3 upVector;
	glm::vec3 cameraTarget;

public:
	MeshViewer(const int width, const int height, TriMesh* mesh) : Window(width, height, "MeshViewer"),
		mesh(mesh) {
		initialize();
	}

	virtual ~MeshViewer() {
	}

	void initialize() {
		vao.create(*mesh);
		shader.create(vert_file, frag_file);
		mesh->computeGravity();
		gravity = mesh->gravity;
		rotCenter = gravity;
		cameraPos = gravity + glm::vec3(100.0f, 0.0f, 0.0f);
		cameraTarget = gravity;
		upVector = glm::vec3(0.0f, 1.0f, 0.0f);
		viewMat = glm::lookAt(cameraPos,  // 視点の位置
			cameraTarget,  // 見ている先
			upVector); // 視界の上方向
		//
		modelMat = glm::translate(glm::vec3(3.0f, 4.0f, 5.0f));
	}

	void draw() override {

		//バインド
		shader.bind();
		vao.bind();

		// Uniform変数の転送
		shader.set_uniform_value(mvpMat(), "u_mvpMat");

		glDrawElements(GL_TRIANGLES, mesh->verIndices.size(), GL_UNSIGNED_INT, 0);

		//アンバインド
		vao.release();
		shader.release();
	}
};

#endif//MESH_VIEWER