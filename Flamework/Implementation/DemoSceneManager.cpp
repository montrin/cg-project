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
    
    _projectionMatrix = perspective(45.0f, 4.0f/3.0f, -1.0f, 100.0f);
    
    //    loadModel("quad.obj", false, false);
    //    loadModel("guy.obj", true, true);
    loadModel("tunnel2.obj", false, false);
    
    
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
            
            shader->setUniform("LightPos", vmml::vec4f(0.f, 1.f, .5f, 1.f));
            //           shader->setUniform("LightPos", _eyePos);
            shader->setUniform("Ia", vmml::vec3f(1.f));
            shader->setUniform("Id", vmml::vec3f(1.f));
            shader->setUniform("Is", vmml::vec3f(1.f));
        }
        else
        {
            util::log("No shader available.", util::LM_WARNING);
        }
        geometry.draw(mode);
    }
}

void DemoSceneManager::draw(double deltaT)
{
    _time += deltaT;
    float angle = _time * .1;   // .1 radians per second
    
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    glCullFace(GL_BACK);
    //    glEnable(GL_CULL_FACE);
    
    Gyro *gyro = Gyro::getInstance();
    gyro->read();
    
    //Camera
    _cameraForward = gyro->getPitch() * 0.5;
    _camera.moveCamera(_cameraForward);
    _viewMatrix = _camera.getViewMatrix();
    
    //Model
    vmml::mat4f translation = vmml::create_translation(vmml::vec3f(_scrolling.x(), -_scrolling.y(), 0));
    
    _modelMatrix = translation;
    
    drawModel("tunnel2");
}
