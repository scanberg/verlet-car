//
//  main.cpp
//  Verlet
//
//  Created by Robin Skånberg on 2012-01-31.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//


#include <iostream>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/gtx/euler_angles.hpp>

#include "VerletSystem.h"
#include "ObjLoader.h"
#include "Camera.h"
#include "Terrain.h"
#include "Vehicle.h"

void setupCamera();
void showFPS();
unsigned int createWheel(VerletSystem &vs, glm::vec3 centrum, float radius, int points, float stiffness, float edgestiffness=0.2f);
int init();

VerletSystem vs;
float oldt;

glm::vec3 camrot;
float movespeed = 0.5;
float mousespeed = 0.4;

int cameraMode = 1;

Camera cam;

unsigned int p,bwheel, fwheel, twheel;
std::pair<unsigned int, unsigned int> wheel;
Vehicle car;

int main (int argc, const char * argv[])
{    
    if(!init())
        return 1;
    
    bool running=true;
    
    Terrain terrain;
    terrain.createFromHeightmap("rockwall_height.tga");
    
    std::set<unsigned int> faceset;
    
    terrain.fillSetWithNearbyFaces(&faceset, glm::vec3(-90.0,0.0,0.0));
    
    car.vs.setTerrain(&terrain);
    
    glm::vec3 p0(0,0,0), p1(0,1,0), p2(1,0,0);
    glm::vec3 pos(0.3,0.3,-1), oldpos(0.3,0.3,1);
    std::cout<<intersectParticleTriangle(oldpos, pos, p0, p1, p2);
    
    float dt = 1.0f/60.0f;
    oldt = (float)glfwGetTime();
    while (running)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        showFPS();
        
        float t = (float)glfwGetTime();
        
        car.setThrottle(0.0f);
        car.setBreaks(0.0f);
        
        if(glfwGetKey(GLFW_KEY_UP))
        {
			car.setThrottle(1.0f);
        }
        if(glfwGetKey(GLFW_KEY_DOWN))
        {
			car.setThrottle(-1.0f);
        }
        if(glfwGetKey(GLFW_KEY_LEFT))
        {
            car.steerLeft();
        }
        if(glfwGetKey(GLFW_KEY_RIGHT))
        {
            car.steerRight();
        }
        
        if(glfwGetKey('E'))
            car.vs.setStaticForce(glm::vec3(0.0,15.0,0.0));
        else
            car.vs.setStaticForce(glm::vec3(0));
        
        car.update(dt);
        
        if(cameraMode == 0)
        {
            //cam.setPosition(car.vs.getCenter());
            //cam.move(-glm::vec3(0.0,5.0,-10));
            glm::vec3 velvec = glm::normalize(car.vs.getAvgVel());
            glm::vec3 movevec = car.vs.getCenter() - velvec*100.0f;
            cam.setPosition(movevec);
            cam.setOrientation(glm::degrees(glm::acos(velvec.y)),glm::degrees(glm::acos(velvec.x)),velvec.x);
        }
        if(cameraMode == 1)
        {
            glm::vec3 movevec;
            
            if(glfwGetKey('W'))
                movevec.z -= movespeed;
            if(glfwGetKey('A'))
                movevec.x -= movespeed;
            if(glfwGetKey('S'))
                movevec.z += movespeed;
            if(glfwGetKey('D'))
                movevec.x += movespeed;
            
            cam.move(movevec);
            
            int x,y;
            glfwGetMousePos(&x, &y);
            
            camrot.x -= (float)(y-240)*mousespeed;
            camrot.y -= (float)(x-320)*mousespeed;
            
            if(camrot.x > 90.0f) camrot.x = 90.0f;
            if(camrot.x < -90.0f) camrot.x = -90.0f;
            if(camrot.y > 360.0f) camrot.y -= 360.0f;
            if(camrot.y < -360.0f) camrot.y += 360.0f;
            
            cam.setOrientation(camrot);
            glfwSetMousePos(320, 240);
        }
        
        oldt = t;
    
        cam.setup();
        glColor3f(1.0, 1.0, 1.0);

        car.draw();
        terrain.draw();
        glfwSwapBuffers();
        
        if(glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED) || glfwGetKey('Q'))
            running = false;
    }
    
    glfwTerminate();
	
    return 0;
}

int init()
{
    glfwInit();
    
    if(!glfwOpenWindow(640, 480, 8, 8, 8, 8, 32, 0, GLFW_WINDOW))
    {
        glfwTerminate();
        return 0;
    }
    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;
    GLenum error = glewInit();
    if(GLEW_OK != error)
    {
        printf("glew error: init returned an error \n");
        glfwTerminate();
        return -1;
    }
    
    //Lite gravity
    car.vs.addStaticAcceleration(glm::vec3(0,-9,0));
    
    //Lite force
    car.vs.addStaticForce(glm::vec3(0,0,0));
    
	loadObj(car, "gris.obj", glm::vec3(0,-8,0), 0.5f);
    
    car.initGL();
    // /Users/scanberg/cpp/Verlet/Verlet/
	//bwheel = createWheel(vs, glm::vec3(-3,0,-8), 2.2f, 16, 0.3f,5.0f);
    
    //car.vs.setSpringColor(280, glm::vec3(1.0,0.0,0.0) );
    
    camrot=glm::vec3(-120,-120,0);
    
    return 1;
}

unsigned int createWheel(VerletSystem &vs, glm::vec3 centrum, float radius, int points, float stiffness, float edgestiffness)
{
    int offset = vs.getNumParticles();
    float v = 2.0*3.14159265358979f / (float)points;
    //std::cout << "# Wheel\n";
    vs.addParticle(centrum,10.0f);
    for(int i=offset+1; i<=offset+points; ++i)
    {
        glm::vec3 vec(radius*glm::cos(i*v),radius*glm::sin(i*v),0);
		//generate vertex list
		//std::cout << "v " << radius*glm::cos(i*v) << " " << radius*glm::sin(i*v) << " 0\n";
        vs.addParticle(centrum+vec,0.5f);
        vs.addToTorqueList(offset, i);
        vs.addSpring(offset, i,stiffness);
        if(i>offset+1)
		{
            vs.addSpring(i-1, i,edgestiffness);
			//generate face list
			//std::cout << "f " << i-1 << " " << i << " 0\n";
		}
    }
    vs.addSpring(offset+1, offset+points,edgestiffness);
    
    return offset;
}

void setupCamera() {
    
    int width, height;
    
    // Get window size. It may start out different from the requested
    // size, and will change if the user resizes the window.
    glfwGetWindowSize( &width, &height );
    if(height<=0) height=1; // Safeguard against iconified/closed window
    
    // Set viewport. This is the pixel rectangle we want to draw into.
    glViewport( 0, 0, width, height ); // The entire window
    
    // Select and setup the projection matrix.
    glMatrixMode(GL_PROJECTION); // "We want to edit the projection matrix"
    glLoadIdentity(); // Reset the matrix to identity
    // 65 degrees FOV, same aspect ratio as window, depth range 1 to 100
    gluPerspective( 60.0f, (GLfloat)width/(GLfloat)height, 1.0f, 1000.0f );
    
    // Select and setup the modelview matrix.
    // Select and setup the modelview matrix.
    glMatrixMode( GL_MODELVIEW ); // "We want to edit the modelview matrix"
    glLoadIdentity(); // Reset the matrix to identity
    glTranslatef(0.0, 0.0, -20.0);
}

/* showFPS() - Calculate and report frames per second
 (updated once per second) in the window title bar
 */
void showFPS() {
    
    float t, fps;
    static float t0 = 0.0;
    static int frames = 0;
    static char titlestring[200];
    
    // Get current time
    t = glfwGetTime();  // Gets number of seconds since glfwInit()
    
    // If one second has passed, or if this is the very first frame
    if( (t-t0) > 1.0 || frames == 0 )
    {
        fps = (float)frames / (t-t0);
        sprintf(titlestring, "Verlet system (%.1f FPS)", fps);
        glfwSetWindowTitle(titlestring);
        t0 = t;
        frames = 0;
    }
    frames ++;
}
