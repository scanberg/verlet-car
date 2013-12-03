//
//  Collision.h
//  Verlet
//
//  Created by Robin Skånberg on 2012-02-16.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Verlet_Collision_h
#define Verlet_Collision_h

#include <glm/glm.hpp>

struct Collision
{
    glm::vec3 position;		// Exakt position där partikeln passerade planet
    float penetration;		// Penetreringdjup, alltid längs normalen sett. 
    glm::vec3 normal;		// Normal till kollisionsplan
    glm::vec3 pointInPlane;	// En punkt som ligger i kollisionsplanet.
};

#endif
