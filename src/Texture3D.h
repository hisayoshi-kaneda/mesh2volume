#pragma once

#ifndef TEXTURE3D_H
#define TEXTURE3D_H

#include "Volume.h"
#include "common.h"

class Texture3D {
public:
	GLuint textureId = 0u;
	GLenum textureUnit;

	Texture3D(int width, int height, int depth, GLint internalformat, GLenum format, GLenum textureUnit = GL_TEXTURE0)
		:textureUnit(textureUnit) {
		initialize();
		glTexImage3D(GL_TEXTURE_3D, 0, internalformat, width, height, depth, 0, format, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_3D, 0);
	}

	Texture3D(Volume& volume) {
		initialize();
		glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, volume.size[0], volume.size[1], volume.size[2], 0, GL_RED, GL_FLOAT, &volume.data[0]);
		glBindTexture(GL_TEXTURE_3D, 0);
	}

	~Texture3D() {
		if (textureId != 0) {
			glDeleteTextures(1, &textureId);
		}
	}

	void initialize() {
		glActiveTexture(textureUnit);
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_3D, textureId);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
		static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, black);
	}

	void bind() {
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, textureId);
	}

	void release() {
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
};

#endif //TEXTURE3D_H