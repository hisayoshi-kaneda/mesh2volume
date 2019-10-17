#include "Volume.h"

Volume::Volume() : size{0, 0, 0}, resolution(1.0), threshold(0), data(nullptr) {}

Volume::Volume(int sizeX, int sizeY, int sizeZ, float resolution, float threshold) : size{sizeX, sizeY, sizeZ}, resolution(resolution), threshold(threshold), data(nullptr) {
    data = new float[size[0] * size[1] * size[2]];
    memset(data, 0, sizeof(float) * size[0] * size[1] * size[2]);
}

Volume::Volume(int sizeX, int sizeY, int sizeZ, float resolution) : size{sizeX, sizeY, sizeZ}, resolution(resolution), data(nullptr) {
    data = new float[size[0] * size[1] * size[2]];
    memset(data, 0, sizeof(float) * size[0] * size[1] * size[2]);
    compute_threshold();
}

Volume::Volume(const Volume &other)
    : size{other.size[0], other.size[1], other.size[2]}, resolution{other.resolution}, threshold(other.threshold), data{nullptr} {
    data = new float[size[0] * size[1] * size[2]];
    std::memcpy(data, other.data, sizeof(float) * size[0] * size[1] * size[2]);
}

Volume::Volume(Volume &&other) noexcept
    : size{other.size[0], other.size[1], other.size[2]}, resolution{other.resolution}, threshold(other.threshold), data(other.data) {
    other.data = nullptr;
}

Volume::~Volume() {
    delete[] data;
}

Volume &Volume::operator=(Volume other) {
    swap(*this, other);
    return *this;
}

void swap(Volume &first, Volume &second) {
    using std::swap;
    swap(first.size, second.size);
    swap(first.resolution, second.resolution);
    swap(first.threshold, second.threshold);
    swap(first.data, second.data);
}

float &Volume::operator()(int x, int y, int z) {
    Assertion(x >= 0 && x < size[0] && y >= 0 && y < size[1] && z >= 0 && z < size[2],
              "Volume subscription is out of bounds!");
    return data[(z * size[1] + y) * size[0] + x];
}

float Volume::operator()(int x, int y, int z) const {
    Assertion(x >= 0 && x < size[0] && y >= 0 && y < size[1] && z >= 0 && z < size[2],
              "Volume subscription is out of bounds!");
    return data[(z * size[1] + y) * size[0] + x];
}

void Volume::standardize() {
    float max_val = *max_element(data, data + size[0] * size[1] * size[2]);
    float min_val = *min_element(data, data + size[0] * size[1] * size[2]);

#pragma omp parallel for
    for (int x = 0; x < size[0]; x++) {
        for (int y = 0; y < size[1]; y++) {
            for (int z = 0; z < size[2]; z++) {
                (*this)(x, y, z) = ((*this)(x, y, z) - min_val) / (max_val - min_val);
            }
        }
    }
    threshold = (threshold - min_val) / (max_val - min_val);
}

void Volume::resize(int sizeX, int sizeY, int sizeZ) {
    delete[] data;

    size[0] = sizeX;
    size[1] = sizeY;
    size[2] = sizeZ;
    resolution = 1.0;

    data = new float[size[0] * size[1] * size[2]];
    std::memset(data, 0, sizeof(float) * size[0] * size[1] * size[2]);
}

float Volume::get_trilinear_interpolation_val(float x, float y, float z) {
    x /= resolution, y /= resolution, z /= resolution;
    int x0 = (int)x, y0 = (int)y, z0 = (int)z;
    int x1 = x0 + 1, y1 = y0 + 1, z1 = z0 + 1;
    float xd = x - (float)x0;
    float yd = y - (float)y0;
    float zd = z - (float)z0;
    float c00 = (*this)(x0, y0, z0) * (1 - xd) + (*this)(x1, y0, z0) * xd;
    float c01 = (*this)(x0, y0, z1) * (1 - xd) + (*this)(x1, y0, z1) * xd;
    float c10 = (*this)(x0, y1, z0) * (1 - xd) + (*this)(x1, y1, z0) * xd;
    float c11 = (*this)(x0, y1, z1) * (1 - xd) + (*this)(x1, y1, z1) * xd;

    float c0 = c00 * (1 - yd) + c10 * yd;
    float c1 = c01 * (1 - yd) + c11 * yd;

    return c0 * (1 - zd) + c1 * zd;
}

void Volume::compute_threshold() {
    float max_val = *max_element(data, data + size[0] * size[1] * size[2]);
    vector<int> histogram((int)max_val + 1, 0);
    for (int x = 0; x < size[0]; x++) {
        for (int y = 0; y < size[1]; y++) {
            for (int z = 0; z < size[2]; z++) {
                int val = (*this)(x, y, z);
                histogram[val]++;
            }
        }
    }
    int w1 = 0, w2 = size[0] * size[1] * size[2];
    int sum1 = 0;
    int sum2 = 0;
    for (int i = 0; i < (int)histogram.size(); i++) {
        sum2 += histogram[i] * i;
    }
    int res = 0;
    float mx = 0;
    for (int i = 0; i < (int)max_val; i++) {
        w1 += histogram[i], sum1 += i * histogram[i], w2 -= histogram[i], sum2 -= i * histogram[i];
        if (w1 == 0)
            continue;
        float m1 = (float)sum1 / (float)w1;
        float m2 = (float)sum2 / (float)w2;
        float tmp = (float)w1 * (float)w2 * (m1 - m2) * (m1 - m2);
        if (mx <= tmp) {
            mx = tmp;
            res = i;
        }
    }
    threshold = (float)res;
}