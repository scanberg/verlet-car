//
//  Camera.cpp
//  Gestapo3
//
//  Created by Robin Skånberg on 2012-01-06.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <GL/glfw.h>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "Camera.h"

float Frustum::distance(const glm::vec4 &plane, const glm::vec3 &point)
{
    return plane.x*point.x + plane.y*point.y + plane.z*point.z + plane.w;
}

int Frustum::pointInFrustum(const glm::vec3 &p) {
    
	int result = INSIDE;
    
	for(int i=0; i < 6; i++) {
		if (distance(plane[i],p) < 0)
			return OUTSIDE;
	}
	return(result);
}

int Frustum::boxInsideFrustum(const AABB &box)
{
    int result = INSIDE;
	//for each plane
	for(int i=0; i < 6; ++i)
    {
		// is the positive vertex outside?
		if (distance(plane[i],box.getVertexP(glm::vec3(plane[i]))) < 0.0f)
			return OUTSIDE;
		// is the negative vertex outside?
		else if (distance(plane[i],box.getVertexN(glm::vec3(plane[i]))) < 0.0f)
			result =  INTERSECT;
	}
	return(result);
}

void Frustum::calculateConstants(float angle, float ratio, float near, float far)
{
    this->ratio = ratio;
    this->angle = angle;
    this->near = near;
    this->far = far;
    
    tang = glm::tan( glm::radians(angle) * 0.5f );
    
    nh = near * tang;
    nw = nh * ratio;
    
    fh = far * tang;
    fw = fh * ratio;
}

glm::vec4 normalizePlane(const glm::vec4 &in)
{
    float t = 1.0f / glm::length(glm::vec3(in));
    return in * t;
}

void Frustum::calculatePlanesbad()
{    
    float   proj[16];
    float   modl[16];
    float   clip[16];
    float   t;
    
    /* Get the current PROJECTION matrix from OpenGL */
    glGetFloatv( GL_PROJECTION_MATRIX, proj );
    
    /* Get the current MODELVIEW matrix from OpenGL */
    glGetFloatv( GL_MODELVIEW_MATRIX, modl );
    
    /* Combine the two matrices (multiply projection by modelview) */
    clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
    clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
    clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
    clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];
    
    clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
    clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
    clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
    clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];
    
    clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
    clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
    clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
    clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];
    
    clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
    clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
    clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
    clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];
    
    /* Extract the numbers for the RIGHT plane */
    plane[0][0] = clip[ 3] - clip[ 0];
    plane[0][1] = clip[ 7] - clip[ 4];
    plane[0][2] = clip[11] - clip[ 8];
    plane[0][3] = clip[15] - clip[12];
    
    /* Normalize the result */
    t = 1.0f / glm::sqrt( plane[0][0] * plane[0][0] + plane[0][1] * plane[0][1] + plane[0][2] * plane[0][2] );
    plane[0] *= t;
    
    /* Extract the numbers for the LEFT plane */
    plane[1][0] = clip[ 3] + clip[ 0];
    plane[1][1] = clip[ 7] + clip[ 4];
    plane[1][2] = clip[11] + clip[ 8];
    plane[1][3] = clip[15] + clip[12];
    
    /* Normalize the result */
    t = 1.0f / glm::sqrt( plane[1][0] * plane[1][0] + plane[1][1] * plane[1][1] + plane[1][2] * plane[1][2] );
    plane[1] *= t;
    
    /* Extract the BOTTOM plane */
    plane[2][0] = clip[ 3] + clip[ 1];
    plane[2][1] = clip[ 7] + clip[ 5];
    plane[2][2] = clip[11] + clip[ 9];
    plane[2][3] = clip[15] + clip[13];
    
    /* Normalize the result */
    t = 1.0f / glm::sqrt( plane[2][0] * plane[2][0] + plane[2][1] * plane[2][1] + plane[2][2] * plane[2][2] );
    plane[2] *= t;
    
    /* Extract the TOP plane */
    plane[3][0] = clip[ 3] - clip[ 1];
    plane[3][1] = clip[ 7] - clip[ 5];
    plane[3][2] = clip[11] - clip[ 9];
    plane[3][3] = clip[15] - clip[13];
    
    /* Normalize the result */
    t = 1.0f / glm::sqrt( plane[3][0] * plane[3][0] + plane[3][1] * plane[3][1] + plane[3][2] * plane[3][2] );
    plane[3] *= t;
    
    /* Extract the FAR plane */
    plane[4][0] = clip[ 3] - clip[ 2];
    plane[4][1] = clip[ 7] - clip[ 6];
    plane[4][2] = clip[11] - clip[10];
    plane[4][3] = clip[15] - clip[14];
    
    /* Normalize the result */
    t = 1.0f / glm::sqrt( plane[4][0] * plane[4][0] + plane[4][1] * plane[4][1] + plane[4][2] * plane[4][2] );
    plane[4] *= t;
    
    /* Extract the NEAR plane */
    plane[5][0] = clip[ 3] + clip[ 2];
    plane[5][1] = clip[ 7] + clip[ 6];
    plane[5][2] = clip[11] + clip[10];
    plane[5][3] = clip[15] + clip[14];
    
    /* Normalize the result */
    t = 1.0f / glm::sqrt( plane[5][0] * plane[5][0] + plane[5][1] * plane[5][1] + plane[5][2] * plane[5][2] );
    plane[5] *= t;
    
    /*for(int i=0; i<6; ++i)
    {
        std::cout<<plane[i].x<<" "<<plane[i].y<<" "<<plane[i].z<<std::endl;
    }*/
}

void Frustum::drawNormals() {
    
    glm::vec3 a,b;
    
	glBegin(GL_LINES);
    
    // near
    a = (ntr + ntl + nbr + nbl) * 0.25f;
    b = a + glm::vec3(plane[PNEAR])*10.0f;
    glVertex3f(a.x,a.y,a.z);
    glVertex3f(b.x,b.y,b.z);
    
    // far
    a = (ftr + ftl + fbr + fbl) * 0.25f;
    b = a + glm::vec3(plane[PFAR])*10.0f;
    glVertex3f(a.x,a.y,a.z);
    glVertex3f(b.x,b.y,b.z);
    
    // left
    a = (ftl + fbl + nbl + ntl) * 0.25f;
    b = a + glm::vec3(plane[PLEFT])*10.0f;
    glVertex3f(a.x,a.y,a.z);
    glVertex3f(b.x,b.y,b.z);
    
    // right
    a = (ftr + nbr + fbr + ntr) * 0.25f;
    b = a + glm::vec3(plane[PRIGHT])*10.0f;
    glVertex3f(a.x,a.y,a.z);
    glVertex3f(b.x,b.y,b.z);
    
    // top
    a = (ftr + ftl + ntr + ntl) * 0.25f;
    b = a + glm::vec3(plane[PTOP])*10.0f;
    glVertex3f(a.x,a.y,a.z);
    glVertex3f(b.x,b.y,b.z);
    
    // bottom
    a = (fbr + fbl + nbr + nbl) * 0.25f;
    b = a + glm::vec3(plane[PBOTTOM])*10.0f;
    glVertex3f(a.x,a.y,a.z);
    glVertex3f(b.x,b.y,b.z);
    
	glEnd();
    
    
}

glm::vec4 createPlane(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
    glm::vec3 aux1, aux2, normal;
    glm::vec4 p;
    
	aux1 = v1 - v2;
	aux2 = v3 - v2;
    
    normal = glm::normalize(glm::cross(aux2,aux1));

	return glm::vec4(normal,-(glm::dot(normal,v2)));
}

void Frustum::calculatePlanes(const glm::vec3 &pos, const glm::vec3 &dir, const glm::vec3 &up)
{
    glm::vec3 X, Y, Z, nc, fc;
    Z = glm::normalize(-dir);
    X = glm::normalize(glm::cross(up,Z));
    Y = glm::cross(Z,X);
    
    nc = pos - Z * near;
    fc = pos - Z * far;
    
    ntl = nc + Y * nh - X * nw;
	ntr = nc + Y * nh + X * nw;
	nbl = nc - Y * nh - X * nw;
	nbr = nc - Y * nh + X * nw;
    
	ftl = fc + Y * fh - X * fw;
	ftr = fc + Y * fh + X * fw;
	fbl = fc - Y * fh - X * fw;
	fbr = fc - Y * fh + X * fw;
    
    plane[PTOP] = createPlane(ntr,ntl,ftl);
	plane[PBOTTOM] = createPlane(nbl,nbr,fbr);
	plane[PLEFT] = createPlane(ntl,nbl,fbl);
	plane[PRIGHT] = createPlane(nbr,ntr,fbr);
	plane[PNEAR] = createPlane(ntl,ntr,nbr);
	plane[PFAR] = createPlane(ftr,ftl,fbl);
    
    std::cout<<"Z "<<Z.x<<" "<<Z.y<<" "<<Z.z<<std::endl;
    
    for(int i=0; i<6; i++)
    {
    	std::cout<<plane[i].x<<" "<<plane[i].y<<" "<<plane[i].z<<std::endl;
    }
}

void Frustum::drawLines() {
    
	glBegin(GL_LINE_LOOP);
	//near plane
    glVertex3f(ntl.x,ntl.y,ntl.z);
    glVertex3f(ntr.x,ntr.y,ntr.z);
    glVertex3f(nbr.x,nbr.y,nbr.z);
    glVertex3f(nbl.x,nbl.y,nbl.z);
	glEnd();
    
	glBegin(GL_LINE_LOOP);
	//far plane
    glVertex3f(ftr.x,ftr.y,ftr.z);
    glVertex3f(ftl.x,ftl.y,ftl.z);
    glVertex3f(fbl.x,fbl.y,fbl.z);
    glVertex3f(fbr.x,fbr.y,fbr.z);
	glEnd();
    
	glBegin(GL_LINE_LOOP);
	//bottom plane
    glVertex3f(nbl.x,nbl.y,nbl.z);
    glVertex3f(nbr.x,nbr.y,nbr.z);
    glVertex3f(fbr.x,fbr.y,fbr.z);
    glVertex3f(fbl.x,fbl.y,fbl.z);
	glEnd();
    
	glBegin(GL_LINE_LOOP);
	//top plane
    glVertex3f(ntr.x,ntr.y,ntr.z);
    glVertex3f(ntl.x,ntl.y,ntl.z);
    glVertex3f(ftl.x,ftl.y,ftl.z);
    glVertex3f(ftr.x,ftr.y,ftr.z);
	glEnd();
    
	glBegin(GL_LINE_LOOP);
	//left plane
    glVertex3f(ntl.x,ntl.y,ntl.z);
    glVertex3f(nbl.x,nbl.y,nbl.z);
    glVertex3f(fbl.x,fbl.y,fbl.z);
    glVertex3f(ftl.x,ftl.y,ftl.z);
	glEnd();
    
	glBegin(GL_LINE_LOOP);
	// right plane
    glVertex3f(nbr.x,nbr.y,nbr.z);
    glVertex3f(ntr.x,ntr.y,ntr.z);
    glVertex3f(ftr.x,ftr.y,ftr.z);
    glVertex3f(fbr.x,fbr.y,fbr.z);
    
	glEnd();
}

void Camera::move(const glm::vec3 &vec)
{
    glm::mat4 rotmat = glm::yawPitchRoll(glm::radians(orientation.y), glm::radians(orientation.x), glm::radians(orientation.z));
    position += glm::vec3( rotmat * glm::vec4(vec,0.0) );
}

int Camera::boxInsideFrustum(const AABB &box)
{
    return frustum.boxInsideFrustum(box);
}

void Camera::draw()
{
    //frustum.drawLines();
    //frustum.drawNormals();
}

void Camera::setup()
{
    // Get window size. It may start out different from the requested
    // size, and will change if the user resizes the window.
    glfwGetWindowSize( &screensize.x, &screensize.y );
    if(screensize.y<=0) screensize.y=1; // Safeguard against iconified/closed window
    
    // Set viewport. This is the pixel rectangle we want to draw into.
    glViewport( 0, 0, screensize.x, screensize.y ); // The entire window
    
    // Select and setup the projection matrix.
    glMatrixMode(GL_PROJECTION); // "We want to edit the projection matrix"
    glLoadIdentity(); // Reset the matrix to identity
    
    GLfloat ratio = (GLfloat)screensize.x/(GLfloat)screensize.y;
    
    gluPerspective( fov, ratio, nearfar.x, nearfar.y );
    
    // Select and setup the modelview matrix.
    // Select and setup the modelview matrix.
    glMatrixMode( GL_MODELVIEW ); // "We want to edit the modelview matrix"
    glLoadIdentity(); // Reset the matrix to identity

    glRotatef(-orientation.x,1.0,0.0,0.0);
    glRotatef(-orientation.y,0.0,1.0,0.0);
    glRotatef(-orientation.z,0.0,0.0,1.0);
    
    glTranslatef(-position.x,-position.y,-position.z); //translate the screen
    
    //frustum.calculateConstants(fov, ratio, nearfar.x, nearfar.y);
    //frustum.calculatePlanes(position,dir,glm::vec3(0.0,1.0,0.0));
    frustum.calculatePlanesbad();
}