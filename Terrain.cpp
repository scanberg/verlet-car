//
//  Terrain.cpp
//  Verlet
//
//  Created by Robin Skånberg on 2012-02-16.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <GL/glew.h>
#include "Terrain.h"

float Terrain::getSample(GLFWimage *img, float x, float y)
{
    x = glm::clamp(x, 0.0f, 1.0f);
    y = glm::clamp(y, 0.0f, 1.0f);
    
    float x0 = (float)glm::floor(x*img->Width)/(float)img->Width;
    float x1 = (float)glm::floor(x*img->Width + 1)/(float)img->Width;
    float y0 = (float)glm::floor(y*img->Height)/(float)img->Height;
    float y1 = (float)glm::floor(y*img->Height + 1)/(float)img->Height;
    
    int width = img->Width;
    int height = img->Height-1;
    
    int i00 = glm::floor(y*height) * width + glm::floor(x*width);
    int i01 = glm::floor(y*height) * width + glm::floor(x*width) + 1;
    int i10 = (glm::floor(y*height) + 1) * width + glm::floor(x*width);
    int i11 = (glm::floor(y*height) + 1) * width + glm::floor(x*width) +1;
    
    i00 = glm::clamp(i00, 0, img->Width*img->Height-1);
    i01 = glm::clamp(i01, 0, img->Width*img->Height-1);
    i10 = glm::clamp(i10, 0, img->Width*img->Height-1);
    i11 = glm::clamp(i11, 0, img->Width*img->Height-1);
    
    float s00 = img->Data[i00]/255.0f;
    float s01 = img->Data[i01]/255.0f;
    float s10 = img->Data[i10]/255.0f;
    float s11 = img->Data[i11]/255.0f;
    
    float fy0 = (x1-x)/(x1-x0)*s00 + (x-x0)/(x1-x0)*s01;
    float fy1 = (x1-x)/(x1-x0)*s10 + (x-x0)/(x1-x0)*s11;
    
    return (y1-y)/(y1-y0)*fy1 + (y-y0)/(y1-y0)*fy0;
}

void Terrain::fillSetWithNearbyFaces(std::set<unsigned int> *testfaces, const glm::vec3 &pos)
{
    // Är partikeln innanför terrängens yta?
    if(pos.x < MAP_X_OFFSET || pos.x > (MAP_X_OFFSET+MAP_X_SCALE))
        return;
    
    if(pos.z < MAP_Z_OFFSET || pos.z > (MAP_Z_OFFSET+MAP_Z_SCALE))
        return;
    
    int x,z;
    unsigned int index;
    
    x = glm::floor( (pos.x-MAP_X_OFFSET)/MAP_X_SCALE * (MAP_X_SEG-1) );
    z = glm::floor( (pos.z-MAP_Z_OFFSET)/MAP_Z_SCALE * (MAP_Z_SEG-1) );
    
    //std::cout<<"faces "<<face.size()<<std::endl;
    
    //std::cout<<"x "<<x<<std::endl;
    //std::cout<<"z "<<z<<std::endl;
    
    index = 2*(z*(MAP_X_SEG-1) + x);
    
    //std::cout<<"index "<<index<<std::endl;
    
    //testfaces->insert(index-1);
    testfaces->insert(index);
    testfaces->insert(index+1);
    //testfaces->insert(index+2);
    
    //testfaces->insert(index + 2*(MAP_X_SEG-1));
    //testfaces->insert(index - 2*(MAP_X_SEG-1)+1);
}

void Terrain::initGL()
{
    processGeometry();
    
    // Generate And Bind The Vertex Buffer
    glGenBuffers( 1, &vbuffer );                  // Get A Valid Name
    glBindBuffer( GL_ARRAY_BUFFER, vbuffer );         // Bind The Buffer
    // Load the vertex data
    glBufferData( GL_ARRAY_BUFFER, vertex.size()*3*sizeof(float), &vertex[0][0], GL_STATIC_DRAW );
    
    glGenBuffers( 1, &nbuffer );                  // Get A Valid Name
    glBindBuffer( GL_ARRAY_BUFFER, nbuffer );         // Bind The Buffer
    // Load the normal data
    glBufferData( GL_ARRAY_BUFFER, normal.size()*3*sizeof(float), &normal[0][0], GL_STATIC_DRAW );
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenBuffers(1, &ibuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
    // Load the face data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * face.size() * 3, &face[0][0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Terrain::processGeometry()
{
    int sharedFaces[vertex.size()];
    int i;
    float numFaces;
    
    glm::vec3 n,a,b;
    
    for (i=0; i<vertex.size(); i++)
    {
        normal.push_back(glm::vec3(0));
        sharedFaces[i] = 0;
    }
    
    for (i=0; i<face.size(); i++)
    {
        a = vertex[face[i].z] - vertex[face[i].x];
        b = vertex[face[i].y] - vertex[face[i].x];
        
        n = glm::cross(b,a);//b.cross(a);
        n = glm::normalize(n);
        
        faceNormal.push_back(n);
        
        normal[face[i].x] += n;
        normal[face[i].y] += n;
        normal[face[i].z] += n;
        
        sharedFaces[face[i].x]++;
        sharedFaces[face[i].y]++;
        sharedFaces[face[i].z]++;
    }
    for (i=0; i<vertex.size(); i++)
    {
        if(sharedFaces[i]>0)
        {
            numFaces = (float)sharedFaces[i];
            normal[i] /= numFaces;
            normal[i] = glm::normalize(normal[i]);
        }
    }
    
}

void Terrain::createFromHeightmap(const std::string &filename)
{
    GLFWimage *img = new GLFWimage();
    
    if(glfwReadImage(filename.c_str(), img, 0)==GL_FALSE)
        return;
    
    for (int z=0; z<MAP_Z_SEG; ++z)
    {
        for (int x=0; x<MAP_X_SEG; ++x)
        {
            vertex.push_back( glm::vec3( 	MAP_X_OFFSET + (float)x/(float)(MAP_X_SEG-1) * MAP_X_SCALE,
                                        MAP_Y_OFFSET + getSample( img, (float)x/(float)(MAP_X_SEG-1), (float)z/(float)(MAP_Z_SEG-1)) * MAP_HEIGHT,
                                        MAP_Z_OFFSET + (float)z/(float)(MAP_Z_SEG-1) * MAP_Z_SCALE ));
            
        }
    }
    
    for (int z=0; z<MAP_Z_SEG-1; ++z)
    {
        for(int x=0; x<MAP_X_SEG-1; ++x)
        {
            face.push_back( glm::uvec3(z*MAP_X_SEG+x,(z+1)*MAP_X_SEG+x, z*MAP_X_SEG+x+1) );
            face.push_back( glm::uvec3((z+1)*MAP_X_SEG+x, (z+1)*MAP_X_SEG+x+1, z*MAP_X_SEG+x+1) );
        }
    }
    
    initGL();
    
    glfwFreeImage(img);
    delete img;
}

void Terrain::draw()
{
    glPolygonMode(GL_FRONT, GL_FILL);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    
    glBindBuffer( GL_ARRAY_BUFFER, vbuffer );
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    
    glBindBuffer( GL_ARRAY_BUFFER, nbuffer);
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
    
    glDrawElements(GL_TRIANGLES, (unsigned int)face.size()*3, GL_UNSIGNED_INT, 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glDisableClientState( GL_VERTEX_ARRAY );
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glDisable(GL_LIGHTING);
}

bool Terrain::getCollision(Collision &col, const glm::vec3 &oldpos, const glm::vec3 &pos)
{
    std::set<unsigned int> faceset;
    fillSetWithNearbyFaces(&faceset, oldpos);
    fillSetWithNearbyFaces(&faceset, pos);
    
//    for(unsigned int i=0; i<face.size(); ++i)
//    {
//        faceset.insert(i);
//    }
    
    std::set<unsigned int>::iterator sit;
    sit = faceset.begin();
    
    glm::vec3 p0, p1, p2, v;
    float t;

    while (sit!=faceset.end())
    {
        p0 = vertex[face[*sit].x];
        p1 = vertex[face[*sit].y];
        p2 = vertex[face[*sit].z];
        
        col.normal = faceNormal[*sit];
        
        col.penetration = glm::dot(col.normal,pos-p0);
        
        if(col.penetration < 0.0f)
        {
        	col.pointInPlane = p0;
        	
        
        	v = oldpos - pos;
        	t = -col.penetration/glm::dot(v,col.normal);
        	col.position = pos + v*t;
        
        	if(barycentricTriangleIntersect(pos+col.normal*col.penetration,p0,p1,p2))
        		return true;
            
        }
        
        sit++;
    }
    
    return false;
}