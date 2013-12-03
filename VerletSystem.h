//
//  VerletSystem.h
//  Verlet
//
//  Created by Robin Skånberg on 2012-01-31.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Verlet_VerletSystem_h
#define Verlet_VerletSystem_h

#include <GL/glfw.h>
#include <vector>
#include <glm/glm.hpp>
#include "Collision.h"
#include "Terrain.h"

//#define FASTSQRT

// Antal iterationer per Spring
#define NUMITERATIONS 20

// "Luftfriktion" procent av velocity som är kvar efter integration
// Viktigt att partiklarna tappar lite energi hela tiden för stabilitet
#define FRICTION 1.0f

class VerletSystem
{
private:
    friend class Vehicle;
    
    // Vektorer för partikel[i]
    std::vector<glm::vec3> position;		// Position
    std::vector<glm::vec3> oldposition;		// Föregående position
    std::vector<glm::vec3> velocity;		// Hastighet
    std::vector<glm::vec3> acceleration;	// Acceleration
    std::vector<glm::vec3> force;			// Kraft
    std::vector<glm::vec3> particleColor;	// Färgvektor (R,G,B) 0.0 - 1.0
    std::vector<float> invmass;				// 1.0 / mass
    
    // Vektorer för spring[i]
    std::vector<std::pair<unsigned int, unsigned int> > spring; // Index för partiklar som spring verkar mellan
    std::vector<float> restlength;								// Vilolängden för fjädrarna
    std::vector<float> restlength2;								// Vilolängd i kvadrat
    std::vector<float> percent;									// Förlängning i procent av restlength
    std::vector<float> stiffness;								// "Styvhet" för fjäder, 0.0 - 1.0
    std::vector<float> suminvmass;								// 1.0 / (mass(p1) + mass(p2))
    std::vector<glm::vec3> springColor;							// Färgvektor (R,G,B) 0.0 - 1.0
    
    // Vridmoment
    std::vector<std::vector< unsigned int > > torquelist;		// Vektor över alla partiklar som påverkas av vridmoment på partikel[i]
    std::vector<unsigned int> torqueAxis;						// Axeln kring vridmomentet beräknas
    
    glm::vec3 staticForce;					// Statisk alltid existerande kraft, verkar på alla partiklar
    glm::vec3 staticAcceleration;			// Statisk alltid existerande acceleration, verkar på alla partiklar
    
    glm::vec3 center;						// Geometriska centrum för bilen, används av kameran
    glm::vec3 avgVel;						// Genomsnittlig hastighetsvektor för bilen, används av kameran
    
    glm::vec3 minWorld, maxWorld;			// min och max värden för vilket partiklarna verkar inom
    
    float dt, dt2, dtratio;					// deltaT, deltaT i kvadrat, deltaT / gamla deltaT
    
    unsigned int numParticles;				// Antal partiklar
    unsigned int numSprings;				// Antal fjädrar
    
    Terrain *terrain;						// Pekare till terrängen
    
    // INTEGRATIONSMETODER
    
    // Newton Strömer Verlet
    void inline nsv(unsigned int i, float dt2)
    {
        acceleration[i] = staticAcceleration + force[i]*invmass[i];
        velocity[i] = FRICTION*(position[i] - oldposition[i]) + acceleration[i] * dt2;
        oldposition[i] = position[i];
        position[i] += velocity[i];
        
        force[i] = staticForce;
    }
    
    // TimeCorrected Verlet, ska vara okänsligare mot förändringar i deltaT
    void inline tcv(unsigned int i, float dtratio, float dt2)
    {
        acceleration[i] = staticAcceleration + force[i]*invmass[i];
        velocity[i] = FRICTION*dtratio*(position[i] - oldposition[i]) + acceleration[i] * dt2;
        oldposition[i] = position[i];
        position[i] += velocity[i];
        
        force[i] = staticForce;
    }
    
    // KOLLISION OCH FRIKTIONS -HANTERING
    
    // Enklaste modellen, flytta partikeln rakt ut längs normalriktingen för ytan
    void resolveCollisionsCrude(unsigned int i);
    
    // Interpolerar mellan en förflyttning längs vektorn pos - oldpos och föregående modell
    void resolveCollisions(unsigned int i);
    
    // Modell som beskrivs i "Cloth.pdf"
    void calculateFriction(unsigned int i);
    
    //kollision uffepuffe
    void calculateFriction2(unsigned int i);
    void resolveTerrainCollisionsFrictionf(unsigned int i);
    
    
    void resolveTerrainCollisions(unsigned int i);
    void resolveTerrainCollisionsCrude(unsigned int i);
    
public:
    // Konstruktorer
    VerletSystem() : dt(1), dt2(1), dtratio(1), minWorld(glm::vec3(-50,-10,-50)), maxWorld(glm::vec3(50,10,50)), terrain(NULL) {};
    
    // Get-metoder
    unsigned int getNumParticles() const { return numParticles; }
    unsigned int getNumSPrings() const { return numSprings; }
    int findSpring(unsigned int particleA, unsigned int particleB);
    
    glm::vec3 getPosition(unsigned int index) { return position[index]; }
    glm::vec3 getVelocity(unsigned int index) { return (position[index]-oldposition[index])/dt; }
    glm::vec3 getAcceleration(unsigned int index) { return acceleration[index]; }
    glm::vec3 getCenter() { return center; }
    glm::vec3 getAvgVel() { return avgVel; }
    float getMass(unsigned int index) { return 1.0f/invmass[index]; }
    
    float getRestlength(unsigned int index) { return restlength[index]; }
    float getPercent(unsigned int index) { return percent[index]; }
    
    float getAngularVelocity(unsigned int index);
    
    // Set-metoder
    
    unsigned int addParticle(const glm::vec3 &v, float mass=1.0f);
    unsigned int addSpring(unsigned int p0, unsigned int p1, float stiff=0.5f);
    
    void addToTorqueList(unsigned int cIndex, unsigned int pIndex);
    void addTorque(unsigned int index, float torque);
    void addStaticForce(const glm::vec3 &v) { staticForce += v; }
    void addForce(unsigned int index, const glm::vec3 &v) { force[index] += v; }
    void addStaticAcceleration(const glm::vec3 &v) { staticAcceleration += v; }
    
    void setStaticForce(const glm::vec3 &v) { staticForce = v; }
    void setAcceleration(unsigned int i, const glm::vec3 &v) { acceleration[i] = v; }
    void setTorqueAxis(unsigned int cIndex, unsigned int springIndex) { torqueAxis[cIndex] = springIndex; }
    void setParticleColor(unsigned int index, const glm::vec3 &col) { particleColor[index] = col; }
    void setSpringColor(unsigned int index, const glm::vec3 &col) { springColor[index] = col; }
    void setPercent(unsigned int index, float per);
    
    void setBounds(const glm::vec3 &min, const glm::vec3 &max) { minWorld=min; maxWorld=max; }
    void setTerrain(Terrain *t) { terrain = t; }
    
    // Uppdatera
    void update(float _dt);
    
    // Rita
    void draw();

};

#endif
