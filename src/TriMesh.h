#pragma once

#ifndef TRI_MESH
#define TRI_MESH

#include"common.h"

class TriMesh {
public:
	unsigned int faceN = 0;
	unsigned int verN = 0;
	unsigned int uvN = 0;
	unsigned int uvfaceN = 0;
	vector<glm::vec3> vertices;
	vector<unsigned int> verIndices;
	vector<glm::vec3> verNormals;
	vector<glm::vec3> faceNormals;
	vector <glm::vec3> faceCenters;
	vector <glm::vec2> uvs;
	vector <unsigned int> uvIndices;
	glm::vec3 gravity = { 0.0f,0.0f,0.0f };

	TriMesh() = default;

	virtual ~TriMesh() {}

	void addVertex(glm::vec3 vertex) {
		vertices.push_back(vertex);
		verN++;
	}

	void addFace(unsigned int index[3]) {
		verIndices.push_back(index[0]);
		verIndices.push_back(index[1]);
		verIndices.push_back(index[2]);
		faceN++;
	}

	void adduv(glm::vec2 uv) {
		uvs.push_back(uv);
		uvN++;
	}

	void adduvFace(unsigned int index[3]) {
		uvIndices.push_back(index[0]);
		uvIndices.push_back(index[1]);
		uvIndices.push_back(index[2]);
		uvfaceN++;
	}

	void computeFaceNormals() {
		faceNormals.clear();
		faceNormals.resize(faceN);
#pragma omp parallel for
		for (int i = 0; i < (int)faceN; i++) {
			unsigned int index[3] = { verIndices[3 * i + 0],verIndices[3 * i + 1],verIndices[3 * i + 2] };
			glm::vec3 v1 = vertices[index[1]] - vertices[index[0]];
			glm::vec3 v2 = vertices[index[2]] - vertices[index[1]];
			faceNormals[i] = normalize(cross(v1, v2));
		}
	}

	void computeVerNormals() {
		verNormals.clear();
		verNormals.resize(verN,glm::vec3(0.0f));
//#pragma omp parallel for
		for (int i = 0; i < (int)faceN; i++) {
			unsigned int index[3] = { verIndices[3 * i + 0],verIndices[3 * i + 1],verIndices[3 * i + 2] };
			glm::vec3 v1 = vertices[index[1]] - vertices[index[0]];
			glm::vec3 v2 = vertices[index[2]] - vertices[index[1]];
			verNormals[index[0]] += cross(v1, v2);
			verNormals[index[1]] += cross(v1, v2);
			verNormals[index[2]] += cross(v1, v2);
		}
//#pragma omp parallel for
		for (int i = 0; i < (int)verN; i++) {
			verNormals[i] = normalize(verNormals[i]);
		}
	}

	void computeFaceCenters() {
		faceCenters.clear();
		faceCenters.resize(faceN);
#pragma omp parallel for
		for (int i = 0; i < faceN; i++) {
			unsigned int index[3] = { verIndices[3 * i + 0],verIndices[3 * i + 1],verIndices[3 * i + 2] };
			faceCenters[i] = (vertices[index[0]] + vertices[index[1]] + vertices[index[2]]) / 3.0f;
		}
	}

	void computeGravity() {
		for (int i = 0; i < verN; i++) {
			gravity += vertices[i] / (float)verN;
		}
	}
};

#endif //TRI_MESH