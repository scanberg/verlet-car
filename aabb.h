//
//  aabb.h
//  Kdtest
//
//  Created by Robin Skånberg on 2012-01-17.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Kdtest_aabb_h
#define Kdtest_aabb_h

#include <glm/glm.hpp>

class AABB
{
public:
    glm::vec3 min, max;
    
    glm::vec3 getVertexP(const glm::vec3 &normal) const
    {
        
        glm::vec3 p = min;
        if (normal.x >= 0.0f)
            p.x = max.x;
        if (normal.y >= 0.0f)
            p.y = max.y;
        if (normal.z >= 0.0f)
            p.z = max.z;
        
        return p;
    }
    
    glm::vec3 getVertexN(const glm::vec3 &normal) const
    {
        glm::vec3 n = max;
        if (normal.x >= 0.0f)
            n.x = min.x;
        if (normal.y >= 0.0f)
            n.y = min.y;
        if (normal.z >= 0.0f)
            n.z = min.z;
        
        return n;
    }
    
    void draw()
    {
        glBegin(GL_LINES);
        
        glVertex3f(min.x, min.y, min.z);
        glVertex3f(max.x, min.y, min.z);
        
        glVertex3f(min.x, min.y, min.z);
        glVertex3f(min.x, max.y, min.z);
        
        glVertex3f(min.x, min.y, min.z);
        glVertex3f(min.x, min.y, max.z);
        
        glVertex3f(max.x, max.y, max.z);
        glVertex3f(min.x, max.y, max.z);
        
        glVertex3f(max.x, max.y, max.z);
        glVertex3f(max.x, min.y, max.z);
        
        glVertex3f(max.x, max.y, max.z);
        glVertex3f(max.x, max.y, min.z);
        
        glVertex3f(min.x, max.y, min.z);
        glVertex3f(max.x, max.y, min.z);
        
        glVertex3f(max.x, min.y, min.z);
        glVertex3f(max.x, max.y, min.z);
        
        glVertex3f(min.x, min.y, max.z);
        glVertex3f(min.x, max.y, max.z);
        
        glVertex3f(min.x, min.y, max.z);
        glVertex3f(max.x, min.y, max.z);
        
        glVertex3f(min.x, max.y, min.z);
        glVertex3f(min.x, max.y, max.z);
        
        glVertex3f(max.x, min.y, min.z);
        glVertex3f(max.x, min.y, max.z);
        
        glEnd();
    }
};

#endif
