//
//  VerletSystem.cpp
//  Verlet
//
//  Created by Robin Skånberg on 2012-02-15.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "VerletSystem.h"

void VerletSystem::setPercent(unsigned int index, float per)
{
    percent[index] = per;
    restlength2[index] = restlength[index]*restlength[index]*per*per;
}

unsigned int VerletSystem::addParticle(const glm::vec3 &v, float mass)
{
    position.push_back(v);
    oldposition.push_back(v);
    force.push_back(glm::vec3(0));
    velocity.push_back(glm::vec3(0));
    acceleration.push_back(glm::vec3(0));
    invmass.push_back(1.0f/mass);
    particleColor.push_back(glm::vec3(0.0f,1.0f,0.0f));
    torqueAxis.push_back(0);
    torquelist.push_back(std::vector<unsigned int>());
    numParticles++;
    
    return numParticles-1;
}

int VerletSystem::findSpring(unsigned int particleA, unsigned int particleB)
{
    for(unsigned int i = 0; i<numSprings; ++i)
    {
        if(spring[i].first == particleA)
        {
            if(spring[i].second == particleB)
                return i;
        }
        if(spring[i].first == particleB)
        {
            if(spring[i].second == particleA)
                return i;
        }
    }
    
    return -1;
}

void VerletSystem::addToTorqueList(unsigned int cIndex, unsigned int pIndex)
{
    if(pIndex == cIndex)
        return;
    
    std::vector< unsigned int >::iterator vit;
    for(vit = torquelist[cIndex].begin(); vit != torquelist[cIndex].end(); vit++)
    {
        if( (*vit) == pIndex )
            return;
    }
    //std::cout<<"adding "<<pIndex<<" to TL "<<cIndex<<std::endl;
    torquelist[cIndex].push_back(pIndex);
}

unsigned int VerletSystem::addSpring(unsigned int p0, unsigned int p1, float stiff)
{
    spring.push_back(std::pair<unsigned int, unsigned int>(p0,p1));
    glm::vec3 diff = (position[p0]-position[p1]);
    restlength.push_back(glm::length(diff));
    restlength2.push_back( glm::dot(diff,diff) );
    percent.push_back(1.0f);
    stiffness.push_back(stiff);
    suminvmass.push_back( invmass[p0] + invmass[p1] );
    springColor.push_back(glm::vec3(1.0f));
    numSprings++;
    
    return numSprings-1;
}

void VerletSystem::addTorque(unsigned int index, float torque)
{
    glm::vec3 v, planeN, f;
    unsigned int i,p;
    
    float scale;
    
    if (torquelist[index].size()==0) {
        return;
    }
    
    scale = 1.0f/(float)torquelist[index].size();
    
    for(i=0; i<torquelist[index].size(); ++i)
    {
        //particleColor[torquelist[index][i]] = glm::vec3(1.0,1.0,0.0);
        p = torquelist[index][i];
        
        v = position[p] - position[index];
        
        planeN = position[spring[torqueAxis[index]].second] - position[spring[torqueAxis[index]].first];
        
        //std::cout<<"planeN x "<<planeN.x<<" y "<<planeN.y<<" z "<<planeN.z<<std::endl;
        f = glm::normalize(glm::cross(planeN,v));
        force[p] += f*scale*(torque/glm::length(v));
    }
}

// Simulerar rotationshastigheten för en partikel. Inte rätt atm, fix it!
float VerletSystem::getAngularVelocity(unsigned int index)
{
    if (torquelist[index].size() == 0)
        return 0.0f;
    
    float omega=0.0f;
    unsigned int i,p;
    glm::vec3 v, planeN, tang, vel;
    float val;
    
    for(i=0; i<torquelist[index].size(); ++i)
    {
        //particleColor[torquelist[index][i]] = glm::vec3(1.0,1.0,0.0);
        p = torquelist[index][i];
        
        v = position[p] - position[index];
        
        planeN = position[spring[torqueAxis[index]].second] - position[spring[torqueAxis[index]].first];
        
        //std::cout<<"planeN x "<<planeN.x<<" y "<<planeN.y<<" z "<<planeN.z<<std::endl;
        tang = glm::normalize(glm::cross(planeN,v));
        
        vel = (position[i] - oldposition[i])/dt;
        
        val = glm::sign(glm::dot(vel,tang))*glm::abs(glm::dot(vel,tang)) / glm::length(v);
        omega += val;
        //std::cout<<"val "<<val<<std::endl;
    }
    omega /= (float)torquelist[index].size();
    
    return omega;
}

// Enklaste modellen, flytta partikeln rakt ut längs normalriktingen för ytan
void VerletSystem::resolveCollisionsCrude(unsigned int i)
{
    glm::vec3 normal;
    float penetration;
    
    for(int u=0; u<2; ++u)
    {
        normal = glm::vec3(0);
        
        if(position[i][u] < minWorld[u])
        {
            normal[u] = 1.0;
            penetration = glm::dot(position[i]-minWorld,normal);
        }
        if(position[i][u] > maxWorld[u])
        {
            normal[u] = -1.0;
            penetration = glm::dot(position[i]-maxWorld,normal);
        }
        
        position[i] -= penetration*normal;
    }
}

// Enklaste modellen, flytta partikeln rakt ut längs normalriktingen för ytan
void VerletSystem::resolveTerrainCollisionsCrude(unsigned int i)
{
    Collision col;
    glm::vec3 veltang, velort;
    //std::cout<<"Kollar efter coll! "<<std::endl;
    if (terrain->getCollision(col, oldposition[i], position[i]))
    {
        velocity[i] = position[i] - oldposition[i];
        velort = glm::dot(velocity[i],col.normal)*col.normal;
        veltang = velocity[i] - velort;
    	position[i] -= col.penetration*col.normal;
        
        velocity[i] = 0.0f*veltang;
        oldposition[i] = position[i] - velocity[i];
        //position[i] = col.position;
        //std::cout<<"normal "<<col.normal.x<<" "<<col.normal.y<<" "<<col.normal.z<<std::endl;
        //std::cout<<"pen "<<col.penetration<<std::endl;
    }
    
}

// Modell som beskrivs i "Game Developer conference" från uffepuffe
void VerletSystem::resolveTerrainCollisionsFrictionf(unsigned int i)
{
    float frictionCoeff = 1.0f;
    Collision col;
    glm::vec3 velperp, velort, penetrationforce, frictionforce;
    //std::cout<<"Kollar efter coll! "<<std::endl;
    if (terrain->getCollision(col, oldposition[i], position[i]))
    {
        velocity[i] = getVelocity(i);
        velort = glm::dot(velocity[i],col.normal)*col.normal;
    	velperp = velort - velocity[i];
        
        float springk = 5000.0f;
        float dampingk = 5.0f;
        
        float dpen1 = col.penetration;//glm::dot(col.penetration, col.normal);
        float dpen2 = glm::dot(velocity[i],col.normal);
        penetrationforce = (springk*dpen1-dampingk*dpen2)*col.normal;
        
        frictionforce = frictionCoeff*glm::length(penetrationforce)*glm::normalize(velperp);
        addForce(i, frictionforce);
        
        
        //Flytta ut position så partikeln inte penetrerar
        position[i] -= col.penetration*col.normal;
        
        //position[i] = col.position;
        //std::cout<<"normal "<<col.normal.x<<" "<<col.normal.y<<" "<<col.normal.z<<std::endl;
        //std::cout<<"pen "<<col.penetration<<std::endl;
    }
    
}



// Interpolerar mellan en förflyttning längs vektorn pos - oldpos och föregående modell
void VerletSystem::resolveCollisions(unsigned int i)
{
    glm::vec3 penetration;
    glm::vec3 u,v, p0;
    glm::vec3 n;
    glm::vec3 pos0, pos1;
    float t;
    bool collision;
    float friction = 0.3f; //Mellan 0 och 1.0
    
    for(int u=0; u<2; ++u)
    {
        penetration = glm::vec3(0);
        collision = false;
        
        if(position[i][u] < minWorld[u])
        {
            penetration[u] = position[i][u]-minWorld[u];
            collision = true;
            n = glm::vec3(0);
            n[u] = 1.0f;
            p0 = minWorld;
        }
        if(position[i][u] > maxWorld[u])
        {
            penetration[u] = position[i][u]-maxWorld[u];
            collision = true;
            n = glm::vec3(0);
            n[u] = -1.0f;
            p0 = maxWorld;
        }
        
        //position[i] -= penetration;
        if(collision)
        {
            v = position[i] - oldposition[i];
            t = (glm::dot(n,p0) - glm::dot(n,oldposition[i]))/glm::dot(v,n);
            
            pos0 = oldposition[i] + v*t;
            pos1 = position[i] - penetration;
            
            //Interpolera mellan rullning utan glidning och totalglidning med friction
            
            position[i] = pos0*friction + pos1*(1.0f-friction);
            
            //std::cout<<"v x"<<v.x<<" y "<<v.y<<" z "<<v.z<<std::endl;
            //std::cout<<"t "<<t<<std::endl;
            
            //oldposition[i] = oldposition[i] - 2.0f * glm::dot(oldposition[i]-position[i],n)*n;
        }
    }
}

// Modell som beskrivs i "Cloth.pdf"
void VerletSystem::calculateFriction(unsigned int i)
{
    float frictionCoeff = 20.0f;
    float dp = 0.0f;
    glm::vec3 fvec;
    glm::vec3 penetration;
    glm::vec3 vel, velort, velpar, n;
    bool collision = false;
    
    //För varje axel, xyz
    for(int u=0; u<2; ++u)
    {
        collision = false;
        penetration = glm::vec3(0);
        
        if(position[i][u] < minWorld[u])
        {
            //Skapa penetreringsvektor, alltid i normalens riktning till ytan.
            penetration[u] = position[i][u]-minWorld[u];
            collision=true;
        }
        if(position[i][u] > maxWorld[u])
        {
            penetration[u] = position[i][u]-maxWorld[u];
            collision=true;
        }
        
        if(collision)
        {
            //Skapa den aktuella velocity vektorn pos - oldpos;
            glm::vec3 vel = position[i] - oldposition[i];
            
            //penetreringsdjupet
            dp = glm::length(penetration);
            
            //skapa en normal till ytan
            n = -penetration/dp;
            
            //Dela upp velocity i två vektorer, en ortogonal med ytan och en parallell
            velort = n * glm::dot(vel,n);
            velpar = vel - velort;
            
            glm::vec3 oldvelort, oldvelpar;
            
            oldvelort = n * glm::dot(velocity[i],n);
            
            //float deltavel = glm::length(penetration);
            
            //Skala nuvarande velocity, dvs oldposition i förhållande till position
            //oldposition[i] = position[i] - velpar * frictionCoeff - velort;
            
            //force[i] -= glm::normalize(velpar)*glm::length(velort)*400.0f/dt;
            
            float c = glm::max( 1.0f - frictionCoeff*dp/glm::length(velpar), 0.0f);
            
            velpar = c*velpar;
            
            //Flytta ut position så partikeln inte penetrerar
            position[i] -= penetration;
            
            oldposition[i] = position[i] - velpar;
        }
    }
}

// Modell som beskrivs i "Game Developer conference" från uffepuffe
void VerletSystem::calculateFriction2(unsigned int i)
{
    float frictionCoeff = 1.0f;
    float dp = 0.0f;
    glm::vec3 fvec, fpen;
    glm::vec3 penetration;
    glm::vec3 vel, velort, velpar, n, velperp;
    bool collision = false;
    
    //För varje axel, xyz
    for(int u=0; u<2; ++u)
    {
        collision = false;
        penetration = glm::vec3(0);
        
        if(position[i][u] < minWorld[u])
        {
            //Skapa penetreringsvektor, alltid i normalens riktning till ytan.
            penetration[u] = position[i][u]-minWorld[u];
            collision=true;
        }
        if(position[i][u] > maxWorld[u])
        {
            penetration[u] = position[i][u]-maxWorld[u];
            collision=true;
        }
        
        if(collision)
        {
            //Skapa den aktuella velocity vektorn pos - oldpos;
            glm::vec3 vel = getVelocity(i);
            
            //penetreringsdjupet
            dp = glm::length(penetration);
            
            //skapa en normal till ytan
            n = -penetration/dp;
            
            //Dela upp velocity i två vektorer, en ortogonal med ytan och en parallell
            velort = n * glm::dot(vel,n);
            velpar = vel - velort;
            
            
            
            
            
            
            //force[i] -= glm::normalize(velpar)*glm::length(velort)*400.0f/dt;
            
            //float c = glm::max( 1.0f - frictionCoeff*glm::length(dp)/glm::length(velpar), 0.0f);
            
            //om v-tangent. * fric.co > deltav enligt cloth
            //if (frictionCoeff*glm::length(velpar) > glm::length(dp)){
            //    c=0;
            //}
            velperp = velort - vel;
            float springk = 1000.0f;
            float dampingk = 50.0f;
            float dpen1 = glm::dot(penetration,n);
            float dpen2 = glm::dot(vel,n);
            fpen = (springk*dpen1-dampingk*dpen2)*n;
            
            fvec = frictionCoeff*glm::length(fpen)*glm::normalize(velperp);
            addForce(i, fvec);
            //Flytta ut position så partikeln inte penetrerar
            position[i] -= penetration;
        }
    }
}

void VerletSystem::update(float _dt)
{
    dtratio = _dt/dt;
    dt2 = _dt*_dt;
    dt = _dt;
    
    glm::vec3 acc, temp;
    
    center = glm::vec3(0);
    avgVel = glm::vec3(0);
    
    for(unsigned int i=0; i<numParticles; ++i)
    {
        nsv(i,dt2);
        center += position[i];
        avgVel += velocity[i];
    }
    
    center /= (float)numParticles;
    avgVel /= (float)numParticles;
    
    unsigned int p0, p1;
    glm::vec3 delta;
    glm::vec3 val;
    
    for(int n=0; n<NUMITERATIONS; ++n)
    {
        for(unsigned int i=0; i<numSprings; ++i)
        {
            p0 = spring[i].first;
            p1 = spring[i].second;
            
            delta = position[p1] - position[p0];
            
            //delta *= (rld - 2.0f*restlength2[i]) / (rld * suminvmass[i]);
            delta *= (1.0f - 2.0f*restlength2[i]/(restlength2[i] + glm::dot(delta,delta))) / suminvmass[i];
            
            val = stiffness[i]*delta;
            
            position[p0] += val*invmass[p0];
            position[p1] -= val*invmass[p1];
        }
    }
    
    if(terrain)
    {
    	for(unsigned int i=0; i<numParticles; ++i)
    	{
        //calculateFriction(i);
        	resolveTerrainCollisionsFrictionf(i);
            //resolveTerrainCollisionsCrude(i);
    	}
    }
    else
    {
        for(unsigned int i=0; i<numParticles; ++i)
    	{
            calculateFriction2(i);
        	//resolveCollisions(i);
    	}
    }
}



void VerletSystem::draw()
{
    //glDisable(GL_DEPTH_TEST);
    glDisable(GL_DEPTH);
    unsigned int p0, p1;
    glm::vec3 forceVec;
    
    //Fjädrar
    glBegin(GL_LINES);
    for(unsigned int i=0; i<numSprings; ++i)
    {
        glColor3fv(&springColor[i][0]);
        p0 = spring[i].first;
        p1 = spring[i].second;
        glVertex3fv(&position[p0][0]);
        glVertex3fv(&position[p1][0]);
    }
    glEnd();
    
//    //Krafter
//    glBegin(GL_LINES);
//    for(unsigned int i=0; i<numParticles; ++i)
//    {
//        glColor3f(1.0,0.0,0.0); 
//        glVertex3fv(&position[i][0]);
//        forceVec = position[i] + force[i]*0.1f;
//        glVertex3fv(&forceVec[0]);
//    }
//    glEnd();
    
    //Partiklar
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    for(unsigned int i=0; i<numParticles; ++i)
    {
        glColor3fv(&particleColor[i][0]);
        glVertex3fv(&position[i][0]);
    }
    glEnd();
}