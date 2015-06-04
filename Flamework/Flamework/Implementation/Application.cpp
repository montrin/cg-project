//
//  Application.cpp
//  Framework
//
//  Created by Rahul Mukhi on 12/5/12.
//  Reworked by David Steiner
//

#include "Application.h"
#include "DemoSceneManager.h"
#include "StartSceneManager.h"

Application::Application()
    : _sceneManager(new DemoSceneManager(this))
     ,_physicsManager(new PhysicsManager(this))
//    , _startSceneManager (new StartSceneManager(this))
{
    
}

Application::~Application()
{
    delete _sceneManager;
    delete _physicsManager;
    //delete _startSceneManager;
}

void Application::initialize(size_t width, size_t height)
{
    _sceneManager->initialize(width, height);
    _physicsManager->initialize();
      //  _startSceneManager->initialize(width, height);
    
}

void Application::update(double deltaT)
{
    _physicsManager->updateSimulation(deltaT);
    
    
}

void Application::draw(double deltaT)
{
   // _startSceneManager->draw(deltaT);
    _sceneManager->draw(deltaT,false);
}

void Application::draw(double deltaT, bool nightView)
{
    _sceneManager->draw(deltaT, nightView);
}

void Application::destroy()
{
    
}

void Application::addTouchHandler(ITouchHandler *arg)
{
    _touchHandlers.insert(arg);
}

void Application::addScaleHandler(IScaleHandler *arg)
{
    _scaleHandlers.insert(arg);
}

void Application::onTouchBegan(float x, float y)
{
    if (_touchHandlers.size() > 0)
    {
        for (auto i = _touchHandlers.begin(); i != _touchHandlers.end(); ++i)
        {
            (*i)->onTouchBegan(x, y);
        }
    }
}

void Application::onTouchMoved(float x, float y)
{
    if (_touchHandlers.size() > 0)
    {
        for (auto i = _touchHandlers.begin(); i != _touchHandlers.end(); ++i)
        {
            (*i)->onTouchMoved(x, y);
        }
    }
}

void Application::onTouchEnded(float x, float y, int tapCount)
{
    if (_touchHandlers.size() > 0)
    {
        for (auto i = _touchHandlers.begin(); i != _touchHandlers.end(); ++i)
        {
            (*i)->onTouchEnded(x, y, tapCount);
        }
    }
}

void Application::onScaleBegan(float x, float y)
{
    if (_scaleHandlers.size() > 0)
    {
        for (auto i = _scaleHandlers.begin(); i != _scaleHandlers.end(); ++i)
        {
            (*i)->onScaleBegan(x, y);
        }
    }
}

void Application::onScaleMoved(float x, float y)
{
    if (_scaleHandlers.size() > 0)
    {
        for (auto i = _scaleHandlers.begin(); i != _scaleHandlers.end(); ++i)
        {
            (*i)->onScaleMoved(x, y);
        }
    }
}

void Application::onScaleEnded(float x, float y)
{
    if (_scaleHandlers.size() > 0)
    {
        for (auto i = _scaleHandlers.begin(); i != _scaleHandlers.end(); ++i)
        {
            (*i)->onScaleEnded(x, y);
        }
    }
}
