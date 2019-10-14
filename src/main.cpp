#include "MeshViewer.h"
#include "SinogramGenerator.h"
#include "SinogramViewer.h"
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
    filename = "data/bunny.obj";
    //filename = "data/lattice-sample2.stl";
    //filename = "data/bunny_watertight.obj";
    TriMeshLoader loader;
    TriMesh mesh = loader.load(filename);
    MeshViewer viewer(WINSIZE_WIDTH, WINSIZE_HEIGHT, &mesh);
    //SinogramViewer viewer(WINSIZE_WIDTH, WINSIZE_HEIGHT, &mesh);
    viewer.main_loop();
    //SinogramGenerator sg(WINSIZE_WIDTH, WINSIZE_HEIGHT, &mesh);
    //sg.generate(1000);
}