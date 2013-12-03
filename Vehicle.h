//
//  Vehicle.h
//  Verlet
//
//  Created by Robin Skånberg on 2012-02-12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Verlet_Vehicle_h
#define Verlet_Vehicle_h


#define MINSTEER -0.05f
#define MAXSTEER 0.05f
#define STEERSPEED 0.003f
#define DRAGCOEFF 1.0f

//#define DRAWVS

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/glfw.h>
#include "VerletSystem.h"

class Vehicle
{
private:
    std::vector<unsigned int>::iterator vit;
    
    GLuint vbuffer, ibuffer, tbuffer, texture;
    
    float steering;
    float steeringSpeed;
    glm::vec3 windVector;
    
    void calculateDrag();
public:
    VerletSystem vs;
    
    std::vector<unsigned int> drivingwheel;
    std::vector<unsigned int> breakwheel;
    std::vector<unsigned int> handbreakwheel;
    std::vector<unsigned int> leftsteer;
    std::vector<unsigned int> rightsteer;

    std::vector<glm::uvec3> triangles;
    std::vector<glm::vec2> texcoord;
    
    Vehicle() {};
    
    void accelerate();
    
    void steerLeft();
    
    void steerRight();
    
    void setThrottle(float percent=1.0f);
    
    void setBreaks(float percent=1.0f);
    
    void setSteering(float val);
    
    void update(float dt);
    
    void initGL();
    
    void draw();
    
    void loadTexture(const std::string &filepath);
};

#endif
