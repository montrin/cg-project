//
//  Camera.h
//  Framework
//
//  Created by Rahul Mukhi on 1/16/13.
//

#ifndef FRAMEWORK_CAMERA_H
#define FRAMEWORK_CAMERA_H

#include <memory>
#include <iostream>	
#include "vector.hpp"
#include "matrix.hpp"
#include "addendum.hpp"

class Camera
{
public:
    
    Camera();
    ~Camera();
    
    vmml::mat4f lookAt(vmml::vec3f eye, vmml::vec3f target, vmml::vec3f up);
    void moveCamera(const float camSpeed);
    void rotateCamera(const vmml::vec3f &axis, float rotation);
    void resetCamera();
    
    vmml::mat4f getViewMatrix();
    
private:
    
    vmml::vec3f _position, _view;
    float _speed;
    vmml::mat4f _rotation = vmml::mat4f::IDENTITY;

};

typedef std::shared_ptr<Camera> CameraPtr;

#endif
