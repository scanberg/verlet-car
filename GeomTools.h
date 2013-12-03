//
//  GeomTools.h
//  Verlet
//
//  Created by Robin Skånberg on 2012-02-08.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Verlet_GeomTools_h
#define Verlet_GeomTools_h

#include <iostream>
#include <glm/glm.hpp>
#include <GL/glfw.h>
#include <vector>

bool barycentricTriangleIntersect(const glm::vec3 &p, const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c);

bool intersectParticleTriangle(const glm::vec3 &oldpos, const glm::vec3 &pos, const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2);

#endif
