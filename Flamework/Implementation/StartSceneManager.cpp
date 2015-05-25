//
//  StartSceneManager
//  Framework
//
//  Created by Monica Trink 25 May 15.
//
//

#include "StartSceneManager.h"
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


StartSceneManager::StartSceneManager(Application *application)
: SceneManager(application)
, _time(24)
, _scaling(1, 1)
, _scrolling(0, 0.25)
{
    
}

StartSceneManager::~StartSceneManager() {}

void StartSceneManager::onTouchBegan(float x, float y)
{
    util::log("onTouchBegan");
    vmml::vec2f cScrollPos(x, y);
    _lScrollPos = cScrollPos;
    
    //    getSound("test")->play();
}

void StartSceneManager::onTouchMoved(float x, float y)
{
    util::log("onTouchMoved");
    vmml::vec2f cScrollPos(x, y);
    vmml::vec2f scrollDelta(cScrollPos - _lScrollPos);
    _scrolling += scrollDelta * SCROLL_SPEED;
    _lScrollPos = cScrollPos;
}

void StartSceneManager::onTouchEnded(float x, float y, int tapCount)
{
    util::log("onTouchEnded");
    _camera.resetCamera();
}

void StartSceneManager::onScaleBegan(float x, float y)
{
    util::log("onScaleBegan");
    vmml::vec2f cScale(-x, y);
    _lScale = cScale;
}

void StartSceneManager::onScaleMoved(float x, float y)
{
    util::log("onScaleModev");
    vmml::vec2f cScale(-x, y);
    vmml::vec2f scaleDelta(cScale - _lScale);
    _scaling += scaleDelta * SCALE_SPEED;
    _lScale = cScale;
}

void StartSceneManager::onScaleEnded(float x, float y)
{
    vmml::vec2f cScale(-x, y);
    vmml::vec2f scaleDelta(cScale - _lScale);
    _scaling += scaleDelta * SCALE_SPEED;
    _lScale = cScale;
}

void StartSceneManager::initialize(size_t width, size_t height)
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
    
    loadModel("sky.obj", true, true);
    //    loadSound("test.mp3");
}


vmml::mat4f StartSceneManager::perspective(float fov, float aspect, float near, float far)
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

void StartSceneManager::drawModel(const std::string &name, GLenum mode)
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
            
            
        }
        else
        {
            util::log("No shader available.", util::LM_WARNING);
        }
        geometry.draw(mode);
        
    }
}

void StartSceneManager::pushModelMatrix()
{
    if(_modelMatrixStack.size() == 0)
    {
        _modelMatrixStack.push(vmml::mat4f::IDENTITY);
    }
    vmml::mat4f topTemp = _modelMatrixStack.top();
    _modelMatrixStack.push(topTemp);
}

void StartSceneManager::popModelMatrix()
{
    if(_modelMatrixStack.size() > 1)
    {
        _modelMatrixStack.pop();
        _modelMatrix = _modelMatrixStack.top();
    }
    
}

void StartSceneManager::transformModelMatrix(const vmml::mat4f &t)
{
    _modelMatrixStack.top() = _modelMatrix * t;
    _modelMatrix = _modelMatrixStack.top();
}

void StartSceneManager::useShader(const std::string &shaderName, const std::string &modelName)
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

void StartSceneManager::draw(double deltaT)
{
    _time += deltaT;
    float angle = _time * .1;   // .1 radians per second
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
    
//    GLint oldFBO;
//    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
//    util::log(boost::lexical_cast<std::string>(oldFBO));
//
    
    //Sun
    pushModelMatrix();
    transformModelMatrix(vmml::create_translation(vmml::vec3f(_scrolling.x(), -_scrolling.y(), 0)));
    transformModelMatrix(vmml::create_translation(vmml::vec3f(0.0, 10.0, 10.0)));
    useShader("sky", "sky");
    drawModel("sky");
    popModelMatrix();
    useShader("test", "sky");
    drawModel("sky");
    popModelMatrix();

}
