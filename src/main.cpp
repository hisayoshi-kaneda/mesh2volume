#include "MeshViewer.h"
#include "Mesh2Volume.h"
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
    //filename = "data/bunny.obj";
    filename = "data/lattice-sample2.stl";
    //filename = "data/lattice-sample.obj";
    //filename = "data/bunny_watertight.obj";
    TriMeshLoader loader;
    TriMesh mesh = loader.load(filename);
    Mesh2Volume m2v(500,500,500,12.0f/500.0f, &mesh);
	//m2v.main_loop();
	Volume volume = m2v.generateVolume();
	m2v.~Mesh2Volume();
	VolumeViewer viewer(WINSIZE_WIDTH, WINSIZE_HEIGHT, &volume);
	viewer.main_loop();
	//string tmp = "meshvolume";
	//volume.write(tmp);
}