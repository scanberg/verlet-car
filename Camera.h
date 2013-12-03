//
//  Camera.h
//  Gestapo3
//
//  Created by Robin Skånberg on 2012-01-06.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Gestapo3_Camera_h
#define Gestapo3_Camera_h

#include <glm/glm.hpp>
#include "aabb.h"

enum {
    PRIGHT 	= 0,
    PLEFT	= 1,
    PBOTTOM = 2,
    PTOP 	= 3,
    PFAR 	= 4,
    PNEAR 	= 5
};
enum {
    OUTSIDE, INTERSECT, INSIDE
};

class Frustum
{
private:
    glm::vec3 nbl, nbr, ntl, ntr, fbl, fbr, ftl, ftr;
    float tang, near, far, angle, ratio, nw, nh, fw, fh;
public:
    glm::vec4 plane[6];
    void calculatePlanesbad();
    void calculatePlanes(const glm::vec3 &pos, const glm::vec3 &dir, const glm::vec3 &up);
    void calculateConstants(float angle, float ratio, float near, float far);
    float distance(const glm::vec4 &plane, const glm::vec3 &point);
    void drawNormals();
    void drawLines();
    int boxInsideFrustum(const AABB &box);
    int pointInFrustum(const glm::vec3 &p);
};

class Camera
{
private:
    glm::vec2 nearfar;
    glm::ivec2 screensize;
    
    float fov;
    
    glm::vec3 position;
    glm::vec3 orientation;
    
public:
    Frustum frustum;
    void setup();
    void draw();
    Camera() : nearfar(glm::vec2(1.0f,1000.0f)), screensize(glm::ivec2(640,480)), fov(60.0f) {}
    void setPosition(const glm::vec3 &pos) { position = pos; }
    void setPosition(float x, float y, float z) { position = glm::vec3(x,y,z);}
    void setOrientation(const glm::vec3 &ori) { orientation = ori; }
    void setOrientation(float x, float y, float z) { orientation = glm::vec3(x,y,z); }
    
    void move(const glm::vec3 &vec);
    
    int boxInsideFrustum(const AABB &box);
    glm::vec3 getPosition() { return position; }
    glm::vec3 getOrientation() { return orientation; }
};

#endif
