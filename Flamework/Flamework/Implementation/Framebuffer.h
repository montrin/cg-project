//
//  Framebuffer.h
//  Flamework
//
//  Created by Simon Müller on 11.05.15.
//
//

#ifndef FRAMEWORK_FRAMEBUFFER_H
#define FRAMEWORK_FRAMEBUFFER_H

#include <string>
#include "Framework_GL.h"
#include "TextureData.h"
#include "Texture.h"

class Framebuffer {
    
private:
    GLuint FBO;
    GLuint colorTexture;
    GLuint depthTexture;
    
    //delete objects
    void destroy();
    
    //generate an empty color texture
    void generateColorTexture(unsigned int fboWidth, unsigned int fboHeight);
    
    //generate depth buffer
    void setupDepthRenderBuffer(unsigned int fboWidth, unsigned int fboHeight);
public:
    Framebuffer();
    ~Framebuffer();
    
    //Generate FBO and two empty textures
    void generateFBO(unsigned int fboWidth, unsigned int fboHeight);
    
    //return color texture from the framebuffer
    GLuint getColorTexture();
    void textureBind(GLint texUnit = GL_TEXTURE0);
    //return depth texture from the framebuffer
    GLuint getDepthTexture();
    
    //destroy and recreate FBO on resize window
    void resize(unsigned int fboWidth, unsigned int fboHeight);
    
    //bind framebuffer to pipeline. We will  call this method in the render loop
    void bind();
				
    //unbind framebuffer from pipeline. We will call this method in the render loop
    void unbind();
    
    // sets the texture of the fbo to GL_TEXTURE(iBuffer)
    void setActiveTexture(int iBuffer);
};

#endif