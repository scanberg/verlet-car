//
//  Vehicle.cpp
//  Verlet
//
//  Created by Robin Skånberg on 2012-03-11.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "Vehicle.h"

void Vehicle::calculateDrag()
{
    glm::vec3 surfaceNormal, forceVector, surfaceVel;
    float area;
    float speed;
    
    glm::vec3 a,b;
    
    for (unsigned int i=0; i<triangles.size(); ++i)
    {
        surfaceVel = windVector-(vs.getVelocity(triangles[i].x) + vs.getVelocity(triangles[i].y) + vs.getVelocity(triangles[i].z))/3.0f;
        speed = glm::length(surfaceVel);
        
        if (!(speed>0.0f))
            return;
            
        surfaceVel /= speed;
        
        a = vs.position[triangles[i].z] - vs.position[triangles[i].x];
        b = vs.position[triangles[i].y] - vs.position[triangles[i].x];
        
        if(glm::dot(a,a) > 0.0f && glm::dot(b,b))
        {
            surfaceNormal = glm::cross(b,a);
            
            //std::cout<<"force "<<surfaceNormal.x<<" "<<surfaceNormal.y<<" "<<surfaceNormal.z<<std::endl;
            if(glm::dot(surfaceNormal,surfaceNormal) > 0.0f)
            {
                area = glm::length(surfaceNormal)/2.0f;
                surfaceNormal /= area*2.0f;
                
                forceVector = (glm::max(glm::dot(surfaceVel,surfaceNormal),0.0f) * area * speed * speed * DRAGCOEFF * surfaceVel) / 3.0f;
                
                vs.addForce(triangles[i].x,forceVector);
                vs.addForce(triangles[i].y,forceVector);
                vs.addForce(triangles[i].z,forceVector);
            }
        }
    }
}

void Vehicle::accelerate()
{
    
}

void Vehicle::steerLeft()
{
    steering -= STEERSPEED;
    steering = glm::max(steering,MINSTEER);
}

void Vehicle::steerRight()
{
    steering += STEERSPEED;
    steering = glm::min(steering,MAXSTEER);
}

void Vehicle::setThrottle(float percent)
{
    /*for (vit = drivingwheel.begin(); vit != drivingwheel.end(); vit++)
     {
     vs.addTorque(*vit,-100);
     }*/
    float torque = 1000;
    float rt = (steering - MAXSTEER)/MAXSTEER;
    float lt = (steering - MINSTEER)/MAXSTEER;
    //std::cout<<"lt "<<lt<<std::endl;
    //std::cout<<"rt "<<rt<<std::endl;
    vs.addTorque(drivingwheel[0], -percent*torque*lt);
    vs.addTorque(drivingwheel[1], -percent*torque*rt);
    vs.addTorque(drivingwheel[2], -percent*torque*lt);
    vs.addTorque(drivingwheel[3], -percent*torque*rt);
}

void Vehicle::setBreaks(float percent)
{  
    /*for (vit = drivingwheel.begin(); vit != drivingwheel.end(); vit++)
     {
     vs.addTorque(*vit,100);
     }*/
    //vs.addTorque(drivingwheel[0], -percent*300);
    //vs.addTorque(drivingwheel[1], -percent*300);
}

void Vehicle::setSteering(float val)
{
    //std::cout<<val<<std::endl;
    for(vit = leftsteer.begin(); vit != leftsteer.end(); vit++)
    {
        vs.setPercent((*vit), 1.0f + val);
    }
    for(vit = rightsteer.begin(); vit != rightsteer.end(); vit++)
    {
        vs.setPercent((*vit), 1.0f - val);
    }
}

void Vehicle::update(float dt)
{
    if(steering > 0.0f)
        steering -= STEERSPEED*0.5f;
    if(steering < 0.0f)
        steering += STEERSPEED*0.5f;
    if(glm::abs(steering) < STEERSPEED*0.5) steering = 0.0f;
    
    //std::cout<<"steering "<<steering<<std::endl;
    
    setSteering(steering);
    
    //calculateDrag();
    vs.update(dt);
        
    glBindBuffer( GL_ARRAY_BUFFER, vbuffer );
    glBufferSubData(GL_ARRAY_BUFFER, 0, vs.numParticles*3*sizeof(float), &vs.position[0][0]);
}

void Vehicle::initGL()
{
    // Generate And Bind The Vertex Buffer
    glGenBuffers( 1, &vbuffer );                  // Get A Valid Name
    glBindBuffer( GL_ARRAY_BUFFER, vbuffer );         // Bind The Buffer
    // Load the vertex data
    glBufferData( GL_ARRAY_BUFFER, vs.numParticles*3*sizeof(float), &vs.position[0][0], GL_DYNAMIC_DRAW );
    
    //glGenBuffers( 1, &nbuffer );                  // Get A Valid Name
    //glBindBuffer( GL_ARRAY_BUFFER, nbuffer );         // Bind The Buffer
    // Load the normal data
    //glBufferData( GL_ARRAY_BUFFER, normal.size()*3*sizeof(float), &normal[0][0], GL_STATIC_DRAW );
    
    glGenBuffers( 1, &tbuffer );
    glBindBuffer( GL_ARRAY_BUFFER, tbuffer );
    // Load the texturedata
    glBufferData( GL_ARRAY_BUFFER, texcoord.size()*2*sizeof(float), &texcoord[0][0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenBuffers(1, &ibuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
    // Load the face data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * triangles.size() * 3, &triangles[0][0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Vehicle::draw()
{
//    glPolygonMode(GL_FRONT, GL_FILL);
//    //glShadeModel(GL_SMOOTH);
//    //glEnable(GL_LIGHTING);
//    //glEnable(GL_LIGHT0);
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, texture);
//    
//    glBindBuffer( GL_ARRAY_BUFFER, vbuffer );
//    glEnableClientState(GL_VERTEX_ARRAY);
//    glVertexPointer(3, GL_FLOAT, 0, 0);
//    
//    //glBindBuffer( GL_ARRAY_BUFFER, nbuffer);
//    //glEnableClientState(GL_NORMAL_ARRAY);
//    //glNormalPointer(GL_FLOAT, 0, 0);
//    
//    glBindBuffer( GL_ARRAY_BUFFER, tbuffer);
//    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//    glTexCoordPointer(2, GL_FLOAT, 0, 0);
//    
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
//    
//    glDrawElements(GL_TRIANGLES, (unsigned int)triangles.size()*3, GL_UNSIGNED_INT, 0);
//    
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//    
//    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
//    
//    glDisableClientState( GL_VERTEX_ARRAY );
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    
//    glBindTexture(GL_TEXTURE_2D, 0);
    
    vs.draw();
}

void Vehicle::loadTexture(const std::string &filepath)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glfwLoadTexture2D(filepath.c_str(), GLFW_BUILD_MIPMAPS_BIT);
    glBindTexture(GL_TEXTURE_2D, 0);
}