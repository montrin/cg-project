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
#include "Framebuffer.h"
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
    
    GLint oldFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
    util::log(boost::lexical_cast<std::string>(oldFBO));
    
    sc_exposure = 0.003; //overall intensity
    sc_decay = 1.0; //dissipation of each sample's contribution as ray progresses away from light source [0,1]
    sc_density = 0.84; //control separation between samples
    sc_weight = 5.65; //intensity of each sample
   
    _camera.moveCamera(_cameraForward);
//  _camera.rotateCamera(vmml::vec3f::UNIT_Y, _cameraRotation);
    _projectionMatrix = perspective(70.0f, 4.0f/3.0, -1.0f, 100.0f);
    
    loadModel("quad2.obj", false, false);
    loadModel("plane.obj", true, true);
    loadModel("sky.obj", true, true);
    loadModel("tunnel7.obj", true, true);
    loadModel("sphere.obj", true, true);
//  loadSound("test.mp3");
    
    fbo.generateFBO(768, 1024);
    fbo2.generateFBO(768, 1024);
    fbo3.generateFBO(768, 1024);
    fbo4.generateFBO(768, 1024);
    fbo5.generateFBO(768, 1024);
    fbo6.generateFBO(768, 1024);
    
    srand(static_cast<unsigned>(time(0)));
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

vmml::mat4f DemoSceneManager::ortho(float near, float far) {
    GLint viewPort[4];
    glGetIntegerv(GL_VIEWPORT, viewPort);
    
    float left = 0.0f;
    float right = left + viewPort[2];
    float bottom = 0.0f;
    float top = bottom + viewPort[3];
    
    vmml::mat4f ortho = vmml::mat4f::IDENTITY;
    
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            ortho.at(i, j) = 0.0f;
        }
    }
    
    ortho.at(0, 0) = 2.0f / (right - left);
    ortho.at(0, 3) = -(right + left) / (right - left);
    ortho.at(1, 1) = 2.0f / (top - bottom);
    ortho.at(1, 3) = -(top + bottom) / (top - bottom);
    ortho.at(2, 2) = -2.0f / (far - near);
    ortho.at(2, 3) = -(far + near) / (far - near);
    ortho.at(3, 3) = 1.0f;
    
    return ortho;
}

vmml::vec3f DemoSceneManager::objectToScreen(vmml::vec4f object) {
    vmml::vec3f screenCoords;
    
    //clip space
    vmml::vec4f homogeneous = _projectionMatrix * _viewMatrix * _modelMatrix * object;
    vmml::vec4f dehomogenized;
    
    //projection divide
    dehomogenized.x() = homogeneous.x()/homogeneous.w();
    dehomogenized.y() = homogeneous.y()/homogeneous.w();
    dehomogenized.z() = homogeneous.z()/homogeneous.w();
    dehomogenized.w() = 1;
    
    util::log("homo- x: " + boost::lexical_cast<std::string>(homogeneous.x()) + " y: " + boost::lexical_cast<std::string>(homogeneous.y()) + " z: " + boost::lexical_cast<std::string>(homogeneous.z()) + " w: " + boost::lexical_cast<std::string>(homogeneous.w()));
    
    util::log("dehomo- x: " + boost::lexical_cast<std::string>(dehomogenized.x()) + " y: " + boost::lexical_cast<std::string>(dehomogenized.y()) + " z: " + boost::lexical_cast<std::string>(dehomogenized.z()));
    
    GLint viewPort[4];
    glGetIntegerv(GL_VIEWPORT, viewPort);
    
    //apply viewport
    screenCoords.x() = viewPort[0] + viewPort[2] * (dehomogenized.x()+1)/2;
    screenCoords.y() = viewPort[1] + viewPort[3] * (dehomogenized.y()+1)/2;
    //dont care about depth
    screenCoords.z() = 0;
    
    util::log("screen- x: " + boost::lexical_cast<std::string>(screenCoords.x()) + " y: " + boost::lexical_cast<std::string>(screenCoords.y()) + " z: " + boost::lexical_cast<std::string>(screenCoords.z()));
    
    return screenCoords;
}

void DemoSceneManager::generateOffsets() {
    for(int i = 0; i < 100; i++) {
        //offsets.push_back(static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/0.1)));
    }
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
            //shader->setUniform("LightPos", vmml::vec4f(2.f, 2.f, 23.0f, 0.f));
            shader->setUniform("LightPos", vmml::vec4f(_sunPosOnScreen, 0.f));
            shader->setUniform("LightDir", vmml::vec4f(-2.f, -8.f, -23.0f, 0.f));
//            
//            vmml::mat4f inverse;
//            vmml::compute_inverse(vmml::mat4f(_sunPos), inverse);
//
//            shader->setUniform("LightPos", _sunPos);
//            shader->setUniform("LightDir", inverse);
            shader->setUniform("Ia", vmml::vec3f(1.f));
            shader->setUniform("Id", vmml::vec3f(1.f));
            shader->setUniform("Is", vmml::vec3f(1.f));
            
            shader->setUniform("rt_w", 600);
            shader->setUniform("rt_h", 600);
            shader->setUniform("vx_offset", 10);
            
            //opacity variable for fog shader
            shader->setUniform("opacity", 1.0f);

            shader->setUniform("uTexSource1",0,fbo.getColorTexture());
                        
            shader->setUniform("sceneTex", 1, fbo2.getColorTexture());
            shader->setUniform("sceneTex2", 2, fbo3.getColorTexture());
            
            shader->setUniform("uTexSource2",3,fbo4.getColorTexture());
            shader->setUniform("texAfterBloom", 4, fbo5.getColorTexture());
            shader->setUniform("texBlackWhite", 5, fbo6.getColorTexture());
            
            //for scattering.frag
            shader->setUniform("exposure", sc_exposure);
            shader->setUniform("decay", sc_decay);
            shader->setUniform("density", sc_density);
            shader->setUniform("weight", sc_weight);
            //shader->setUniform("offsets", offsets);
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

void DemoSceneManager::draw(double deltaT, bool nightMode)
{
    _time += deltaT;
    float angle = _time * .1;   // .1 radians per second
    
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Gyro *gyro = Gyro::getInstance();
    gyro->read();
    
    //Camera
    _cameraForward = gyro->getPitch() * 0.5;
    _camera.moveCamera(_cameraForward);
    _viewMatrix = _camera.getViewMatrix();
    
    _modelMatrix = vmml::mat4f::IDENTITY;
    
    fbo.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Sphere
    //fbo.setActiveTexture(0);
//    pushModelMatrix();
//    
//    transformModelMatrix(vmml::create_translation(vmml::vec3f(_scrolling.x(), -_scrolling.y(), 0)));
//    transformModelMatrix(vmml::create_translation(vmml::vec3f(0.0, 10.0, 10.0)));
//    drawModel("sphere");
//    popModelMatrix();
//    
//    //Sky
//    pushModelMatrix();
//    transformModelMatrix(vmml::create_translation(vmml::vec3f(_scrolling.x(), -_scrolling.y(), 0)));
//    drawModel("sky");
//    popModelMatrix();
//
    //eye adaption simulation
    pushModelMatrix();
//  glClearDepthf(1);
    glEnable(GL_DEPTH_TEST);
    glEnable( GL_BLEND );
    glBlendEquation( GL_FUNC_ADD );
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    transformModelMatrix(vmml::create_translation(vmml::vec3f(_scrolling.x(), -_scrolling.y(), 0)));
    useShader("plane","plane");
    drawModel("plane");
    popModelMatrix();
    
    glDisable(GL_BLEND);
    //    //Tunnel
    pushModelMatrix();
    transformModelMatrix(vmml::create_rotation(90.0f, vmml::vec3f(0.0, 1.0, 0.0)));
    transformModelMatrix(vmml::create_scaling(2.0));
//    transformModelMatrix(vmml::create_translation(vmml::vec3f(_scrolling.x(), -_scrolling.y(), 0)));
    useShader("Material003","tunnel7");
    drawModel("tunnel7");
    popModelMatrix();
    
    //fbo.setActiveTexture(-1);
    fbo2.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    fbo.unbind();
    //fbo.setActiveTexture(0);
    pushModelMatrix();
    useShader("bloom1","quad2");
    drawModel("quad2");
    popModelMatrix();
    //fbo2.setActiveTexture(-1);
//    fbo2.unbind();
    fbo3.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    pushModelMatrix();
    useShader("hblur","quad2");
    drawModel("quad2");
    popModelMatrix();

    fbo4.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    pushModelMatrix();
    useShader("vblur","quad2");
    drawModel("quad2");
    popModelMatrix();
    
//    fbo4.unbind();
    fbo5.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    fbo2.bind();
    pushModelMatrix();
    useShader("bloom0","quad2");
    drawModel("quad2");
    popModelMatrix();
    
//------scattering
    //render scene black/white to fbo6
    fbo6.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    pushModelMatrix();
    //sun
 //   transformModelMatrix(vmml::create_translation(vmml::vec3f(_scrolling.x(), -_scrolling.y(), 0)));
    transformModelMatrix(vmml::create_translation(vmml::vec3f(2.0, 8.0, 23.0)));
    _sunPos = _projectionMatrix;
    useShader("white","sphere");
    drawModel("sphere");
    popModelMatrix();
    
    pushModelMatrix();
    transformModelMatrix(vmml::create_translation(vmml::vec3f(30.0, 6.0, 5.0)));
    useShader("black", "sphere");
    drawModel("sphere");
    popModelMatrix();
    
    pushModelMatrix();
    //tunnel
    transformModelMatrix(vmml::create_rotation(90.0f, vmml::vec3f(0.0, 1.0, 0.0)));
    transformModelMatrix(vmml::create_scaling(2.0));
    useShader("black","tunnel7");
    drawModel("tunnel7");
    popModelMatrix();

    //activate scatter shader on fbo6 and blend with fbo5
    fbo5.bind();
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    //convert sun position
    _projectionMatrix = ortho(-1.0f, 100.0f);
    _sunPosOnScreen = objectToScreen(vmml::vec4f(2.0, 8.0, 23.0, 1));
    _sunPosOnScreen.clamp();
    //change to orthographic projection
    
    pushModelMatrix();
    useShader("scattering","quad2");
    drawModel("quad2");
    popModelMatrix();
    glDisable(GL_BLEND);
    
    //change back to perspective projection
    _projectionMatrix = perspective(70.0f, 4.0f/3.0, -1.0f, 100.0f);
    
////
////    //copy fbo5 to screen
    fbo5.unbind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    pushModelMatrix();
    //check if view should render in night mode
    if(nightMode) {
        useShader("night","quad2");
    }else {
        useShader("copy","quad2");
    }
    drawModel("quad2");
    popModelMatrix();

    //    fbo6.bind();
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glClearColor(1.0,1.0,1.0,1.0);
//    pushModelMatrix();
//    //sun
//    transformModelMatrix(vmml::create_translation(vmml::vec3f(_scrolling.x(), -_scrolling.y(), 0)));
//    transformModelMatrix(vmml::create_translation(vmml::vec3f(0.0, 10.0, 10.0)));
//    useShader("white","sphere");
//    drawModel("sphere");
//    popModelMatrix();
//    pushModelMatrix();
//    //tunnel
//    transformModelMatrix(vmml::create_translation(vmml::vec3f(_scrolling.x(), -_scrolling.y(), 0)));
//    useShader("black","tunnel7");
//    drawModel("tunnel7");
//    popModelMatrix();
////
////    //activate scatter shader on fbo6 and blend with fbo5
//    fbo5.bind();
//    glEnable(GL_BLEND);
//    glBlendEquation(GL_FUNC_ADD);
//    glBlendFunc(GL_ONE, GL_ONE);
//    //glClearColor(1.0,1.0,1.0, 1.0);
//    pushModelMatrix();
//    useShader("scattering","quad2");
//    drawModel("quad2");
//    popModelMatrix();
//    glDisable(GL_BLEND);
////
////    //copy fbo5 to screen
//    fbo5.unbind();
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    pushModelMatrix();
//    //check if view should render in night mode
//    if(nightMode) {
//        useShader("night","quad2");
//    }else {
//        useShader("copy","quad2");
//    }
//    drawModel("quad2");
//    popModelMatrix();
//------------

}
