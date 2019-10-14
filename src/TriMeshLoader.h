#pragma once

#ifndef TRI_MESH_LOADER
#define TRI_MESH_LOADER

#include<filesystem>
#include"TriMesh.h"
#include"tinyply.h"
#include "common.h"
using namespace tinyply;

namespace fs = std::filesystem;

class TriMeshLoader {
public:
	TriMeshLoader() = default;

	TriMesh load(string& filepath) {
		TriMesh mesh;
		string extension = fs::path(filepath).extension().string();
		if (extension == ".obj") {
			mesh = loadOBJ(filepath);
		}
		else if (extension == ".ply") {
			mesh = loadPly(filepath);
		}
		else if (extension == ".stl") {
			mesh = loadStl(filepath);
		}
		else {
			fprintf(stderr, "file format or file extension is not valid");
			exit(1);
		}
		mesh.computeFaceCenters();
		mesh.computeFaceNormals();
		return mesh;
	}
private:
	TriMesh loadOBJ(string& filepath);
	TriMesh loadPly(string& filepath);
	TriMesh loadStl(string& filepath);
};

#endif //TRI_MESH_LOADER