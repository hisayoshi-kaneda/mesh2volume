#pragma once

#ifndef FBO_H
#define FBO_H

#include "Texture2D.h"
#include "common.h"

class FrameBufferObject {
public:
    GLuint fboId = 0;
    GLuint depthBufferId = 0;
    vector<GLuint> textureBufferIds;
    vector<GLuint> colorAttachmentIds;
    int width;
    int height;

    FrameBufferObject(int width, int height) : width(width), height(height) {
        initialize();
    }

    ~FrameBufferObject() {
        if (textureBufferIds.size() != 0) {
            for (GLuint tbId : textureBufferIds) {
                glDeleteTextures(1, &tbId);
            }
        }
        if (depthBufferId != 0) {
            glDeleteTextures(1, &depthBufferId);
        }
        if (fboId != 0) {
            glDeleteFramebuffers(1, &fboId);
        }
    }

    void addColorAttachment() {
        //Generate texture buffer
        GLuint textureBufferId;
        glGenTextures(1, &textureBufferId);
        glBindTexture(GL_TEXTURE_2D, textureBufferId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        //glBindTexture(GL_TEXTURE_2D, 0);
        textureBufferIds.push_back(textureBufferId);

        const int colorAttachmentId = (int)colorAttachmentIds.size();
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentId, GL_TEXTURE_2D, textureBufferId, 0);
        colorAttachmentIds.push_back(GL_COLOR_ATTACHMENT0 + colorAttachmentId);
		release();
    }

    void bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    }

    void release() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void bindTexture(int textureIndex) {
        Assertion(textureIndex >= 0 && textureIndex < (int)textureBufferIds.size(), "TextureId is out of bounds");
        glBindTexture(GL_TEXTURE_2D, textureBufferIds[textureIndex]);
    }

    void releaseTexture() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void setRenderTarget(int size , GLenum *bufs) {
		glDrawBuffers(size, bufs);
    }

    void setRenderTarget() {
        glDrawBuffers(1, &colorAttachmentIds[int(colorAttachmentIds.size()) - 1]);
    }

private:
    void initialize() {
		glActiveTexture(GL_TEXTURE0);
		//Generate texture buffer
        GLuint textureBufferId;
        glGenTextures(1, &textureBufferId);
        glBindTexture(GL_TEXTURE_2D, textureBufferId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
        textureBufferIds.push_back(textureBufferId);

        //Generate depth buffer
		glGenTextures(1, &depthBufferId);
		glBindTexture(GL_TEXTURE_2D, depthBufferId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);

        //Generate frame buffer
        glGenFramebuffers(1, &fboId);
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBufferId, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBufferId, 0);
        colorAttachmentIds.push_back(GL_COLOR_ATTACHMENT0);

        //Release frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

#endif //FBO_H