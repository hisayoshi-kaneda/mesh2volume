#include"TriMeshLoader.h"

TriMesh TriMeshLoader::loadOBJ(string& filepath) {
	TriMesh mesh;

	FILE* file = fopen(filepath.c_str(), "r");
	if (file == NULL) {
		printf("File does not exist.");
		exit(1);
	}

	while (1) {
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			mesh.addVertex(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			mesh.adduv(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			//continue;
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			mesh.verNormals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			fpos_t prev, now;
			fgetpos(file, &prev);
			if (fscanf(file, "%d %d %d\n"
				, &vertexIndex[0]
				, &vertexIndex[1]
				, &vertexIndex[2]) == 3) {
				for (int j = 0; j < 3; j++)vertexIndex[j]--;
				mesh.addFace(vertexIndex);
				continue;
			}
			fgetpos(file, &now);
			fseek(file, prev - now, SEEK_CUR);
			if (fscanf(file, "%d//%d %d//%d %d//%d\n"
				, &vertexIndex[0], &normalIndex[0]
				, &vertexIndex[1], &normalIndex[1]
				, &vertexIndex[2], &normalIndex[2]) == 6) {
				for (int j = 0; j < 3; j++)vertexIndex[j]--;
				mesh.addFace(vertexIndex);
				continue;
			}
			fgetpos(file, &now);
			fseek(file, prev - now, SEEK_CUR);
			if (fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n"
				, &vertexIndex[0], &uvIndex[0], &normalIndex[0]
				, &vertexIndex[1], &uvIndex[1], &normalIndex[1]
				, &vertexIndex[2], &uvIndex[2], &normalIndex[2]) == 9) {
				for (int j = 0; j < 3; j++)vertexIndex[j]--;
				mesh.addFace(vertexIndex);
				continue;
			}
			fprintf(stderr, "File can't be read by our simple parser\n");
			exit(1);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}
	cout << "total vertices : " << mesh.verN << endl;
	cout << "total faces : " << mesh.faceN << endl;
	return mesh;
}

TriMesh TriMeshLoader::loadPly(string& filepath) {
	TriMesh mesh;

	std::ifstream ss(filepath, std::ios::binary);
	if (ss.fail()) {
		fprintf(stderr, "File does not exist.");
	}

	PlyFile file;
	file.parse_header(ss);

	std::cout << "........................................................................\n";
	for (auto c : file.get_comments()) std::cout << "Comment: " << c << std::endl;
	for (auto e : file.get_elements())
	{
		std::cout << "element - " << e.name << " (" << e.size << ")" << std::endl;
		for (auto p : e.properties) std::cout << "\tproperty - " << p.name << " (" << tinyply::PropertyTable[p.propertyType].str << ")" << std::endl;
	}
	std::cout << "........................................................................\n";

	// Tinyply treats parsed data as untyped byte buffers. See below for examples.
	std::shared_ptr<PlyData> vertices, normals, faces, texcoords;

	// The header information can be used to programmatically extract properties on elements
	// known to exist in the header prior to reading the data. For brevity of this sample, properties 
	// like vertex position are hard-coded: 
	try { vertices = file.request_properties_from_element("vertex", { "x", "y", "z" }); }
	catch (const std::exception& e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

	try { normals = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }); }
	catch (const std::exception& e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

	try { texcoords = file.request_properties_from_element("vertex", { "u", "v" }); }
	catch (const std::exception& e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

	// Providing a list size hint (the last argument) is a 2x performance improvement. If you have 
	// arbitrary ply files, it is best to leave this 0. 
	try { faces = file.request_properties_from_element("face", { "vertex_indices" }, 3); }
	catch (const std::exception& e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

	file.read(ss);

	if (vertices) std::cout << "\tRead " << vertices->count << " total vertices " << std::endl;
	if (normals) std::cout << "\tRead " << normals->count << " total vertex normals " << std::endl;
	if (texcoords) std::cout << "\tRead " << texcoords->count << " total vertex texcoords " << std::endl;
	if (faces) std::cout << "\tRead " << faces->count << " total faces (triangles) " << std::endl;

	const size_t numVerticesBytes = vertices->buffer.size_bytes();
	mesh.vertices.resize(vertices->count);
	std::memcpy(mesh.vertices.data(), vertices->buffer.get(), numVerticesBytes);
	mesh.verN = vertices->count;
	const size_t numFacesBytes = faces->buffer.size_bytes();
	mesh.verIndices.resize(faces->count * 3);
	std::memcpy(mesh.verIndices.data(), faces->buffer.get(), numFacesBytes);
	mesh.faceN = faces->count;

	return mesh;
}

TriMesh TriMeshLoader::loadStl(string& filepath) {
	TriMesh mesh;

	int faceN = 0;

	FILE* in = fopen(filepath.c_str(), "r+b");
	fseek(in, 80, SEEK_SET);
	fread(&faceN, 4, 1, in);

	vector<glm::vec3> buff;
	char dummy[10];

	int id = 0;
	float n[3], v[3];
	for (int i = 0; i < (int)faceN; i++) {
		fread(n, sizeof(float), 3, in);
		for (int j = 0; j < 3; j++) {
			glm::vec3 vertex;
			fread(v, sizeof(float), 3, in);
			vertex[0] = v[0];
			vertex[1] = v[1];
			vertex[2] = v[2];
			mesh.addVertex(vertex);
		}
		unsigned int index[] = { 3 * i + 0,3 * i + 1,3 * i + 2 };
		mesh.addFace(index);
		fread(dummy, sizeof(char), 2, in);
	}
	fclose(in);

	return mesh;
}