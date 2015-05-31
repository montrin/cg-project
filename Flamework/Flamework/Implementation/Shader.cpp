//
//  Shader.cpp
//  Framework
//
//  Created by Rahul Mukhi on 1/9/13.
//  Reworked by David Steiner
//

#include "Framework_GL.h"

#ifdef USE_GL_ES2

#include <boost/lexical_cast.hpp>

#include "GeometryData.h"
#include "ShaderData.h"
#include "Shader.h"
#include "Util.h"

using boost::lexical_cast;


Shader::Shader(const ShaderData &shaderData)
{
    GLuint vertShader, fragShader;
    
    // Create shader program.
    _programID = glCreateProgram();
    
    if(!compile(&vertShader, GL_VERTEX_SHADER, shaderData.getVertShaderSrc()))
    {
        util::log("Failed to compile vertex shader");
        exit(-1);
    }
    
    if (!compile(&fragShader, GL_FRAGMENT_SHADER, shaderData.getFragShaderSrc()))
    {
        util::log("Failed to compile fragment shader");
        exit(-1);
    }
    
    // Attach vertex shader to program.
    glAttachShader(_programID, vertShader);
    
    // Attach fragment shader to program.
    glAttachShader(_programID, fragShader);
    
    // Bind attribute locations.
    // This needs to be done prior to linking.
    //    glBindAttribLocation(_programID, GLKVertexAttribPosition, "position");
    //    glBindAttribLocation(_programID, GLKVertexAttribNormal, "normal");
    
    // Link program.
    if (!link()) {
        util::log("Failed to link program: " + lexical_cast< std::string >(_programID));
        
        if (vertShader) {
            glDeleteShader(vertShader);
            vertShader = 0;
        }
        if (fragShader) {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (_programID) {
            glDeleteProgram(_programID);
            _programID = 0;
        }
        
        //exit(0);
    }
    
    resetTexUnit();
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &_maxTexUnits);
    util::log("Number of available tex units: " + lexical_cast< std::string >(_maxTexUnits) + ".", util::LM_INFO);
}

Shader::~Shader()
{
    if (_programID) {
        glDeleteProgram(_programID);
        _programID = 0;
    }
}

void Shader::setUniform(const std::string &name, const vmml::vec4f &arg)
{
    glUseProgram(_programID);
    
    GLint loc = findUniformLocation(name);
    if (loc > -1)
    {
        glUniform4fv(loc, 1, arg.begin());
    }
}

void Shader::setUniform(const std::string &name, const vmml::vec3f &arg)
{
    glUseProgram(_programID);
    
    GLint loc = findUniformLocation(name);
    if (loc > -1)
    {
        glUniform3fv(loc, 1, arg.begin());
    }
}

void Shader::setUniform(const std::string &name, const vmml::mat4f &arg)
{
    glUseProgram(_programID);
    
    GLint loc = findUniformLocation(name);
    if (loc > -1)
    {
        glUniformMatrix4fv(loc, 1, false, arg.begin());
    }
}

void Shader::setUniform(const std::string &name, const vmml::mat3f &arg)
{
    glUseProgram(_programID);
    
    GLint loc = findUniformLocation(name);
    if (loc > -1)
    {
        glUniformMatrix3fv(loc, 1, false, arg.begin());
    }
}

void Shader::setUniform(const std::string &name, float arg)
{
    glUseProgram(_programID);
    
    GLint loc = findUniformLocation(name);
    if (loc > -1)
    {
        glUniform1f(loc, arg);
    }
}

void Shader::setUniform(const std::string &name, TexturePtr texture)
{
    glUseProgram(_programID);

    GLint loc = findUniformLocation(name);
    if (loc > -1)
    {
        texture->bind(GL_TEXTURE0 + _cTexUnit);
        glUniform1i(loc, _cTexUnit);
        
        _cTexUnit = (_cTexUnit + 1) % _maxTexUnits;
    }
}

void Shader::setUniform(const std::string &name)
{
    glUseProgram(_programID);
    
}

GLint Shader::registerUniform(const std::string &name)
{
    GLint loc = glGetUniformLocation(_programID, name.c_str());
    
    // add 1 because -1 is returned by glGetUniformLocation if operation fails,
    // but the map's default value is 0.
    _uniformLocations[name] = loc + 1;
    
    return loc;
}

GLint Shader::registerAttrib(const std::string &name, GLint size, GLenum type, GLsizei stride, size_t offset)
{
    GLint loc = glGetAttribLocation(_programID, name.c_str());
    Attrib &attrib = _attribs[name];
    
    // add 1 because -1 is returned by glGetUniformLocation if operation fails,
    // but the map's default value is 0.
    attrib.loc = loc + 1;
    attrib.size = size;
    attrib.type = type;
    attrib.stride = stride;
    attrib.offset = offset;
    
    glEnableVertexAttribArray(loc);
    return loc;
}

GLuint Shader::getProgramID()
{
    return _programID;
}

GLint Shader::findUniformLocation(const std::string &name)
{
    GLint loc = getUniformLocation(name);
    if (loc < 0)
    {
        loc = registerUniform(name);
    }
    if (loc < 0)
    {
        util::log("Couldn't find uniform '" + name + "'.", util::LM_WARNING);
    }
    return loc;
}

GLint Shader::findAttribLocation(const std::string &name, GLint size, GLenum type, GLsizei stride, size_t offset)
{
    GLint loc = getAttribLocation(name);
    if (loc < 0)
    {
        loc = registerAttrib(name, size, type, stride, offset);
    }
    if (loc < 0)
    {
        util::log("Couldn't find attrib '" + name + "'.", util::LM_WARNING);
    }
    return loc;
}

GLint Shader::getUniformLocation(const std::string &name)
{
    return _uniformLocations[name] - 1;     // subtract 1 again
}

GLint Shader::getAttribLocation(const std::string &name)
{
    return _attribs[name].loc - 1;     // subtract 1 again
}

void Shader::resetTexUnit()
{
    _cTexUnit = 0;
}

void Shader::bind()
{
    glUseProgram(_programID);
    
    for (auto i = _attribs.begin(); i != _attribs.end(); ++i)
    {
        const Attrib &attrib = i->second;
        GLint loc = attrib.loc - 1;
        if (loc > -1)
        {
            glVertexAttribPointer(loc, attrib.size, attrib.type, GL_FALSE, attrib.stride, reinterpret_cast<void*>(attrib.offset));
        }
        else
        {
            util::log("Couldn't bind attrib '" + i->first + "' because its location is not valid.", util::LM_ERROR);
        }
    }
    
    resetTexUnit();
}

bool Shader::compile(GLuint* shader, GLenum type, const std::string &src)
{
    GLint status;
    
    if (src.length() < 1) {
        util::log("Failed to load vertex shader");
        return false;
    }
    
    *shader = glCreateShader(type);
    {
        const GLchar *source = src.c_str();
        glShaderSource(*shader, 1, &source, NULL);
    }
    glCompileShader(*shader);
    
    //#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        std::string log(logLength, ' ');
        glGetShaderInfoLog(*shader, logLength, &logLength, &(*log.begin()));
        util::log("Shader compile log:\n" + log);
    }
    //#endif
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        glDeleteShader(*shader);
        return false;
    }
    
    return true;
}

bool Shader::link()
{
    GLint status;
    glLinkProgram(_programID);
    
    //#if defined(DEBUG)
    GLint logLength;
    glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        std::string log(logLength, ' ');
        glGetProgramInfoLog(_programID, logLength, &logLength, &(*log.begin()));
        util::log("Program link log:\n" + log);
    }
    //#endif
    
    glGetProgramiv(_programID, GL_LINK_STATUS, &status);
    if (status == 0) {
        return false;
    }
    
    return true;
}

bool Shader::validate()
{
    GLint logLength, status;
    
    glValidateProgram(_programID);
    glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        std::string log(logLength, ' ');
        glGetProgramInfoLog(_programID, logLength, &logLength, &(*log.begin()));
        util::log("Program validate log:\n" + log);
    }
    
    glGetProgramiv(_programID, GL_VALIDATE_STATUS, &status);
    if (status == 0) {
        return true;
    }
    
    return false;
}

#endif
