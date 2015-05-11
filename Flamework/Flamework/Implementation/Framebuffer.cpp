//
//  Framebuffer.cpp
//  Flamework
//
//  Created by Simon Müller on 11.05.15.
//
//

#include "Framebuffer.h"

Framebuffer::Framebuffer() {
    colorTexture = 0;
    depthTexture = 0;
}

Framebuffer::~Framebuffer() {
    destroy();
}

void Framebuffer::destroy() {
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &colorTexture);
    glDeleteTextures(1, &depthTexture);
}

void Framebuffer::generateColorTexture(unsigned int fboWidth, unsigned int fboHeight) {
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fboWidth, fboHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0/*image initially empty*/);
}

void Framebuffer::setupDepthRenderBuffer(unsigned int fboWidth, unsigned int fboHeight) {
    glGenRenderbuffers(1, &depthTexture);
    glBindRenderbuffer(GL_RENDERBUFFER, depthTexture);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, fboWidth, fboHeight);
}

void Framebuffer::generateFBO(unsigned int fboWidth, unsigned int fboHeight) {
    //Generate a framebuffer object(FBO) and bind it to the pipeline
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    
    generateColorTexture(fboWidth, fboHeight);
    setupDepthRenderBuffer(fboWidth, fboHeight);
    
    //attach to framebuffer/renderbuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0 /*-->mipmap level, 0 is highest?*/);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthTexture);
    
    //Check for FBO completeness
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    
    if(GL_FRAMEBUFFER_COMPLETE != status) {
        std::fprintf(stderr, "Error: incomplete framebuffer\n");
        std::terminate();
    }
    
    //unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Framebuffer::getColorTexture() {
    return colorTexture;
}

GLuint Framebuffer::getDepthTexture() {
    return depthTexture;
}

void Framebuffer::resize(unsigned int fboWidth, unsigned int fboHeight) {
    destroy();
    generateFBO(fboWidth, fboHeight);
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void Framebuffer::unbind() {
    //0 is the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
