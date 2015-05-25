//
//  DemoSceneManager.cpp
//  Framework
//
//  Created by David Steiner on 4/28/14.
//
//

#include "DemoSceneManager.h"
#include "addendum.hpp"

#include "Application.h"
#include "TextureData.h"
#include "ModelData.h"
#include "ShaderData.h"
#include "Framework_GL.h"
#include "Util.h"

#include <boost/lexical_cast.hpp>

using boost::lexical_cast;

#define SCROLL_SPEED    0.002f
#define SCALE_SPEED     0.008f


DemoSceneManager::DemoSceneManager(Application *application)
: SceneManager(application)
, _time(12)
, _scaling(1, 1)
, _scrolling(0, 0.25)
{
    
}

DemoSceneManager::~DemoSceneManager() {}

void DemoSceneManager::onTouchBegan(float x, float y)
{
    util::log("onTouchBegan");
    vmml::vec2f cScrollPos(x, y);
    _lScrollPos = cScrollPos;
    
    //    getSound("test")->play();
}

void DemoSceneManager::onTouchMoved(float x, float y)
{
    util::log("onTouchMoved");
    vmml::vec2f cScrollPos(x, y);
    vmml::vec2f scrollDelta(cScrollPos - _lScrollPos);
    _scrolling += scrollDelta * SCROLL_SPEED;
    _lScrollPos = cScrollPos;
}

void DemoSceneManager::onTouchEnded(float x, float y, int tapCount)
{
    util::log("onTouchEnded");
    _camera.resetCamera();
}

void DemoSceneManager::onScaleBegan(float x, float y)
{
    util::log("onScaleBegan");
    vmml::vec2f cScale(-x, y);
    _lScale = cScale;
}

void DemoSceneManager::onScaleMoved(float x, float y)
{
    util::log("onScaleModev");
    vmml::vec2f cScale(-x, y);
    vmml::vec2f scaleDelta(cScale - _lScale);
    _scaling += scaleDelta * SCALE_SPEED;
    _lScale = cScale;
}

void DemoSceneManager::onScaleEnded(float x, float y)
{
    vmml::vec2f cScale(-x, y);
    vmml::vec2f scaleDelta(cScale - _lScale);
    _scaling += scaleDelta * SCALE_SPEED;
    _lScale = cScale;
}

void DemoSceneManager::initialize(size_t width, size_t height)
{
    getApplication()->addTouchHandler(this);
    getApplication()->addScaleHandler(this);
    
    _modelMatrixStack.push(vmml::mat4f::IDENTITY);
    
    _cameraForward = 0.0f;
    _forwardSpeed = 1.0f;
    _cameraRotation = M_PI_F;
    
    _camera.moveCamera(_cameraForward);
    //_camera.rotateCamera(vmml::vec3f::UNIT_Y, _cameraRotation);
        _projectionMatrix = perspective(70.0f, 4.0f/3.0, -1.0f, 100.0f);
//    _projectionMatrix = vmml::mat4f::IDENTITY;
    
//        loadModel("quad.obj", false, false);
    //    loadModel("guy.obj", true, true);
//    loadModel("tunnel2.obj", true, true);
//    loadModel("tunnel4.obj", true, true);
   loadModel("sky.obj", true, true);
    loadModel("tunnel6.obj", true, true);
    loadModel("sphere.obj", true, true);
    //    loadSound("test.mp3");
}


vmml::mat4f DemoSceneManager::perspective(float fov, float aspect, float near, float far)
{
    vmml::mat4f perspective = vmml::mat4f::IDENTITY;
    
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            perspective.at(i, j) = 0.0f;
        }
    }
    
    float angle = (fov / 180.0f) * M_PI_F;
    float f = 1.0f / tan(angle * 0.5f);
    
    perspective.at(0, 0) = f / aspect;
    perspective.at(1, 1) = f;
    perspective.at(2, 2) = (far + near) / (near - far);
    perspective.at(2, 3) = -1.0f;
    perspective.at(3, 2) = (2.0f * far * near) / (near - far);
    
    return perspective;
}

void DemoSceneManager::drawModel(const std::string &name, GLenum mode)
{
    Model::GroupMap &groups = getModel(name)->getGroups();
    for (auto i = groups.begin(); i != groups.end(); ++i)
    {
        Geometry &geometry = i->second;
        MaterialPtr material = geometry.getMaterial();
        ShaderPtr shader = material->getShader();
        if (shader.get())
        {
            shader->setUniform("ProjectionMatrix", _projectionMatrix);
            shader->setUniform("ViewMatrix", _viewMatrix);
            shader->setUniform("ModelMatrix", _modelMatrix);
            
            vmml::mat3f normalMatrix;
            vmml::compute_inverse(vmml::transpose(vmml::mat3f(_modelMatrix)), normalMatrix);
            shader->setUniform("NormalMatrix", normalMatrix);
            
            shader->setUniform("EyePos", _eyePos);
            
            shader->setUniform("LightPos", vmml::vec4f(2.f, 2.f, 23.0f, 0.f));
            shader->setUniform("LightDir", vmml::vec4f(-2.f, -2.f, -23.0f, 0.f));

//                       shader->setUniform("LightPos", _eyePos);
            shader->setUniform("Ia", vmml::vec3f(1.f));
            shader->setUniform("Id", vmml::vec3f(1.f));
            shader->setUniform("Is", vmml::vec3f(1.f));
            
            shader->setUniform("rt_w", 384);
            shader->setUniform("rt_h", 512);
            shader->setUniform("vx_offset", 5);
            
            
            
            
        }
        else
        {
            util::log("No shader available.", util::LM_WARNING);
        }
        geometry.draw(mode);
        
    }
}

void DemoSceneManager::pushModelMatrix()
{
    if(_modelMatrixStack.size() == 0)
    {
        _modelMatrixStack.push(vmml::mat4f::IDENTITY);
    }
    vmml::mat4f topTemp = _modelMatrixStack.top();
    _modelMatrixStack.push(topTemp);
}

void DemoSceneManager::popModelMatrix()
{
    if(_modelMatrixStack.size() > 1)
    {
        _modelMatrixStack.pop();
        _modelMatrix = _modelMatrixStack.top();
    }
    
}

void DemoSceneManager::transformModelMatrix(const vmml::mat4f &t)
{
    _modelMatrixStack.top() = _modelMatrix * t;
    _modelMatrix = _modelMatrixStack.top();
}

void DemoSceneManager::useShader(const std::string &shaderName, const std::string &modelName)
{
    //getting shader pointer/loading shader if not already loaded
    if(getShader(shaderName).get()) {
        _shader = getShader(shaderName);
    } else {
        _shader = loadShader(shaderName);
    }
    
    //setting new shader for material of specified model
    Model::GroupMap &groups = getModel(modelName)->getGroups();
    for (auto i = groups.begin(); i != groups.end(); ++i)
    {
        Geometry &geometry = i->second;
        MaterialPtr material = geometry.getMaterial();
        material->setShader(_shader);
    }
}

void DemoSceneManager::draw(double deltaT)
{
    _time += deltaT;
    float angle = _time * .1;   // .1 radians per second
    
    glClearColor(0.0f, 0.8f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    Gyro *gyro = Gyro::getInstance();
    gyro->read();
    
    //Camera
    _cameraForward = gyro->getPitch() * 0.5;
    _camera.moveCamera(_cameraForward);
    _viewMatrix = _camera.getViewMatrix();
    
    _modelMatrix = vmml::mat4f::IDENTITY;
    
    GLint oldFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
    util::log(boost::lexical_cast<std::string>(oldFBO));
    
    FramebufferPtr fbo = createFBO();
    fbo->generateFBO(384, 512);
    fbo->bind();

    //Sky
    pushModelMatrix();
    transformModelMatrix(vmml::create_translation(vmml::vec3f(_scrolling.x(), -_scrolling.y(), 0)));
    useShader("sky", "sky");
    drawModel("sky");
    popModelMatrix();

    //render sky in red
    FramebufferPtr fbo2 = createFBO();
    fbo2->generateFBO(384, 512);
    fbo2->bind();
    pushModelMatrix();
    useShader("test", "sky");
    drawModel("sky");
    popModelMatrix();

    
    //horizontal blur
    FramebufferPtr fbo3 = createFBO();
    fbo3->generateFBO(384, 512);
    fbo3->bind();
    pushModelMatrix();
    useShader("hblur", "sky");
    drawModel("sky");
    popModelMatrix();
    
    //<------ final scene ----->
    fbo->unbind(oldFBO);

    //draw final sun
    pushModelMatrix();
    transformModelMatrix(vmml::create_translation(vmml::vec3f(_scrolling.x(), -_scrolling.y(), 0)));
    transformModelMatrix(vmml::create_translation(vmml::vec3f(0.0, 10.0, 10.0)));
    drawModel("sphere");
    popModelMatrix();
    
    //draw final sky
    pushModelMatrix();
    useShader("vblur", "sky");
    drawModel("sky");
    popModelMatrix();

    //    FBO.unbind(oldFBO);

//    useShader("test", "sky");
//    drawModel("sky");
//    popModelMatrix();

//    Framebuffer fbo2;
//    fbo2.generateFBO(384, 512);
//    fbo2.bind();

//    drawModel("sky");

//    FBO.unbind(oldFBO);
    //Tunnel
//    pushModelMatrix();
//    transformModelMatrix(vmml::create_translation(vmml::vec3f(_scrolling.x(), -_scrolling.y(), 0)));
//    useShader("Material003", "tunnel6");
//    drawModel("tunnel6");
//    popModelMatrix();
//    useShader("hblur", "tunnel6");
//    drawModel("tunnel6");
//    popModelMatrix();
//    useShader("vblur", "tunnel6");
//    drawModel("tunnel6");
//    popModelMatrix();
}
