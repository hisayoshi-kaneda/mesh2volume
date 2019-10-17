#pragma once

#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "common.h"

class Texture2D {
public:
	GLuint textureId = 0u;
	GLenum textureUnit;
	Texture2D(int width, int height, GLint internalformat, GLenum format, GLenum textureUnit = GL_TEXTURE0)
		:textureUnit(textureUnit) {
		glActiveTexture(textureUnit);
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	~Texture2D() {
		if (textureId != 0) {
			glDeleteTextures(1, &textureId);
		}
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

#endif //TEXTURE2D_H