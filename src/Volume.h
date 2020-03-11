#ifdef _MSC_VER
#pragma once
#endif

#ifndef VOLUME_H
#define VOLUME_H

#include "common.h"

class Volume {
public:
    int size[3] = {0};
    float resolution = 0;
    float threshold = 0;
    float *data = nullptr;

    Volume();
    Volume(int sizeX, int sizeY, int sizeZ, float resolution, float threshold);
    Volume(int sizeX, int sizeY, int sizeZ, float resolution);
    Volume(const Volume &other);
    Volume(Volume &&other) noexcept;
    virtual ~Volume();

    Volume &operator=(Volume other);

    friend void swap(Volume &first, Volume &second);

    float &operator()(int x, int y, int z);
    float operator()(int x, int y, int z) const;

    void standardize();

    void resize(int sizeX, int sizeY, int sizeZ);

    float get_trilinear_interpolation_val(float x, float y, float z);

    void compute_threshold();

    void write(const char path[]) {
        FILE *out = fopen(path, "w+b");
        fwrite(data, sizeof(float), size[0] * size[1] * size[2], out);
        fclose(out);
    }
};

#endif // VOLUME_H