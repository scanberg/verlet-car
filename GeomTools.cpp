//
//  GeomTools.cpp
//  Verlet
//
//  Created by Robin Skånberg on 2012-02-16.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "GeomTools.h"

bool barycentricTriangleIntersect(const glm::vec3 &p, const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c)
{
    glm::vec3 v0 = c - a;
    glm::vec3 v1 = b - a;
    glm::vec3 v2 = p - a;
    
    double dot00 = (double)glm::dot(v0,v0);
    double dot01 = glm::dot(v0,v1);
    double dot02 = glm::dot(v0,v2);
    double dot11 = glm::dot(v1,v1);
    double dot12 = glm::dot(v1,v2);
    
    double invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
    double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    double v = (dot00 * dot12 - dot01 * dot02) * invDenom;
    
    return (u >= 0.0f) && (v >= 0.0f) && (u + v < 1.0f);
}

bool intersectParticleTriangle(const glm::vec3 &oldpos, const glm::vec3 &pos, const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2)
{
    //kanske inte behöver normalize
    glm::vec3 n = glm::cross(p2-p0,p1-p0);
    glm::vec3 v = oldpos - pos;
    
    if(glm::sign(glm::dot(oldpos,n)) == glm::sign(glm::dot(pos,n)) )
        return false;
        
    float t = glm::dot(n,p0-pos)/glm::dot(n,v);
    
    glm::vec3 p = pos + v*t;
    
    return barycentricTriangleIntersect(p,p0,p1,p2);
}