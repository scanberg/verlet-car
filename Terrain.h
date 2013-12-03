//
//  Terrain.h
//  Verlet
//
//  Created by Robin Skånberg on 2012-02-09.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Verlet_Terrain_h
#define Verlet_Terrain_h

#define MAP_X_OFFSET -100.0f
#define MAP_Y_OFFSET -14.0f
#define MAP_Z_OFFSET -100.0f

#define MAP_X_SCALE 200.0f
#define MAP_Z_SCALE 200.0f

#define MAP_HEIGHT 2.0f

#define MAP_X_SEG 100
#define MAP_Z_SEG 100

#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <vector>
#include <set>
#include "GeomTools.h"
#include "Collision.h"

class Terrain
{
private:
    int size_x, size_z;
	std::vector<glm::vec3> vertex;
    std::vector<glm::vec3> normal;
    std::vector<glm::uvec3> face;
    std::vector<glm::vec3> faceNormal;
    
    GLuint vbuffer, nbuffer, ibuffer;
    GLuint texture[4];
    
    //Returnerar en linjär interpolering mellan sampelpunkter kring x,y
    float getSample(GLFWimage *img, float x, float y);
    
    void initGL();
    
    void processGeometry();
public:
    void createFromHeightmap(const std::string &filename);
    
    void fillSetWithNearbyFaces(std::set<unsigned int> *testfaces, const glm::vec3 &pos);
    
    void draw();
    
    bool getCollision(Collision &col, const glm::vec3 &oldpos, const glm::vec3 &pos);
};

#endif
