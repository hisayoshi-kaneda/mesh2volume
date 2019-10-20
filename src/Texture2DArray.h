#pragma once

#ifndef TEXTURE2DARRAY_H
#define TEXTURE2DARRAY_H

#include "common.h"

class Texture2DArray {
public:
    GLuint textureArrayId = 0u;
    GLenum textureArrayUnit;
    Texture2DArray(int width, int height, int LayerN, GLint internalformat, GLenum format, GLenum textureArrayUnit = GL_TEXTURE0)
        : textureArrayUnit(textureArrayUnit) {
        glActiveTexture(textureArrayUnit);
        glGenTextures(1, &textureArrayId);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayId);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalformat, width, height, LayerN, 0, format, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }

    ~Texture2DArray() {
        if (textureArrayId != 0) {
            glDeleteTextures(1, &textureArrayId);
        }
    }

    void bind() {
        glActiveTexture(textureArrayUnit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayId);
    }

    void release() {
        glActiveTexture(textureArrayUnit);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }
};

#endif //TEXTURE2D_H