#pragma once

#ifndef FBO_H
#define FBO_H

#include "Texture2D.h"
#include "common.h"

class FrameBufferObject {
public:
    GLuint fboId = 0;
    int width;
    int height;

    FrameBufferObject(int width, int height) : width(width), height(height) {
        initialize();
    }

    ~FrameBufferObject() {
        if (fboId != 0) {
            glDeleteFramebuffers(1, &fboId);
        }
    }

	void attachColorTexture(Texture2D& tex) {
		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.textureId, 0);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void attachDepthTexture(Texture2D& tex) {
		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex.textureId, 0);
	}

    void bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    }

    void release() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

private:
    void initialize() {
		glActiveTexture(GL_TEXTURE0);

        //Generate frame buffer
        glGenFramebuffers(1, &fboId);
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);

        //Release frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

#endif //FBO_H