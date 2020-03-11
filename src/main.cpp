#include "Mesh2Volume.h"
#include "MeshViewer.h"
#include "Volume.h"
#include "VolumeViewer.h"
#include "Window.h"
#include "common.h"

static constexpr uint32_t WINSIZE_WIDTH = 1000;
static constexpr uint32_t WINSIZE_HEIGHT = 1000;

int main(int argc, char **argv) {
    // 入力ファイル名
    string filename;
    if (argc <= 1) {
        fprintf(stderr, "[ WARN ] input file is not specified. Use default file.\n");
    } else {
        fprintf(stdout, "[ INFO ] input file: %s\n", argv[1]);
        filename = string(argv[1]);
    }
    TriMeshLoader loader;
    shared_ptr<TriMesh> mesh = make_shared<TriMesh>(loader.load(filename));
    mesh->computeAABB();
	float size[3];
	for (int i = 0; i < 3; i++) {
		size[i] = mesh->maxPointAABB[i] - mesh->minPointAABB[i];
	}
	float maxLength = max(size[0], max(size[1], size[2]));
    Mesh2Volume m2v(512, 512, 512, maxLength / 512.0f, mesh);
    Volume volume = m2v.generateVolume();
    m2v.~Mesh2Volume();
    VolumeViewer viewer(WINSIZE_WIDTH, WINSIZE_HEIGHT, &volume);
    viewer.main_loop();
}
