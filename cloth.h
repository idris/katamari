/*
 Adapted from Jesper Mosegaard's Cloth Tutorial
 ==============================================
 
 This source code is accompanying the Cloth Tutorial at the cg.alexandra.dk blog.
 
 You may use the code in any way you see fit. Please leave a comment on the blog 
 or send me an email if the code or tutorial was somehow helpful.
 
 Everything needed is defined in this file, it is probably best read from the 
 bottom and up, since dependancy is from the bottom and up
 
 A short overview of this file is;
 * includes
 * physics constant
 
 * class Vec3
 * class Particle (with position Vec3)
 * class Constraint (of two particles)
 * class Cloth (with particles and constraints)
 
 * Cloth object and ball (instance of Cloth Class)
 
 * OpenGL/Glut methods, including display() and main() (calling methods on Cloth object)
 
 Jesper Mosegaard, clothTutorial@jespermosegaard.dk
 
 Tested on: Windows Vista / Visual Studio 2005
 Linux (Red Hat) / GCC 4.1.2
 */

#ifndef _CLOTH_H
#define _CLOTH_H

#include <vector>


class Vec3 // a minimal vector class of 3 floats and overloaded math operators
{	
public:
	float f[3];
	
	Vec3(float x, float y, float z);
	
	Vec3();
	
	float length();
	
	Vec3 normalized();
	
	void operator+= (const Vec3 &v);
	
	Vec3 operator/ (const float &a);
	
	Vec3 operator- (const Vec3 &v);
	
	Vec3 operator+ (const Vec3 &v);
	
	Vec3 operator* (const float &a);
	
	Vec3 operator-();
	
	Vec3 cross(const Vec3 &v);
	
	float dot(const Vec3 &v);
};


/* The particle class represents a particle of mass that can move around in 3D space*/
class Particle
{
private:
	bool movable; // can the particle move or not ? used to pin parts of the cloth
	
	float mass; // the mass of the particle (is always 1 in this example)
	Vec3 pos; // the current position of the particle in 3D space
	Vec3 old_pos; // the position of the particle in the previous time step, used as part of the verlet numerical integration scheme
	Vec3 acceleration; // a vector representing the current acceleration of the particle
	Vec3 accumulated_normal; // an accumulated normal (i.e. non normalized), used for OpenGL soft shading
	
public:
	Particle(Vec3 pos);// : pos(pos), old_pos(pos),acceleration(Vec3(0,0,0)), mass(1), movable(true), accumulated_normal(Vec3(0,0,0));
	Particle();
	
	void addForce(Vec3 f);
	
	/* This is one of the important methods, where the time is progressed a single step size (TIME_STEPSIZE)
	 The method is called by Cloth.time_step()
	 Given the equation "force = mass * acceleration" the next position is found through verlet integration*/
	void timeStep();
	
	Vec3& getPos();
	
	void resetAcceleration();
	
	void offsetPos(const Vec3 v);
	
	void makeUnmovable();
	
	void addToNormal(Vec3 normal);
	
	Vec3& getNormal();
	
	void resetNormal();

};


class Constraint
{
private:
	float rest_distance; // the length between particle p1 and p2 in rest configuration
	
public:
	Particle *p1, *p2; // the two particles that are connected through this constraint
	
	Constraint(Particle *p1, Particle *p2);
	
	/* This is one of the important methods, where a single constraint between two particles p1 and p2 is solved
	 the method is called by Cloth.time_step() many times per frame*/
	void satisfyConstraint();
	
};


class Cloth
{
private:
	
	int num_particles_width; // number of particles in "width" direction
	int num_particles_height; // number of particles in "height" direction
	// total number of particles is num_particles_width*num_particles_height
	
	std::vector<Particle> particles; // all particles that are part of this cloth
	std::vector<Constraint> constraints; // alle constraints between particles as part of this cloth
	
	Particle* getParticle(int x, int y);
	void makeConstraint(Particle *p1, Particle *p2);
	
	
	/* A private method used by drawShaded() and addWindForcesForTriangle() to retrieve the  
	 normal vector of the triangle defined by the position of the particles p1, p2, and p3.
	 The magnitude of the normal vector is equal to the area of the parallelogram defined by p1, p2 and p3
	 */
	Vec3 calcTriangleNormal(Particle *p1,Particle *p2,Particle *p3);
	
	/* A private method used by windForce() to calcualte the wind force for a single triangle 
	 defined by p1,p2,p3*/
	void addWindForcesForTriangle(Particle *p1,Particle *p2,Particle *p3, const Vec3 direction);
	
	/* A private method used by drawShaded(), that draws a single triangle p1,p2,p3 with a color*/
	void drawTriangle(Particle *p1, Particle *p2, Particle *p3, const Vec3 color);
	
public:
	
	/* This is a important constructor for the entire system of particles and constraints*/
	Cloth(float width, float height, int num_particles_width, int num_particles_height);
	
	/* drawing the cloth as a smooth shaded (and colored according to column) OpenGL triangular mesh
	 Called from the display() method
	 The cloth is seen as consisting of triangles for four particles in the grid as follows:
	 
	 (x,y)   *--* (x+1,y)
	 | /|
	 |/ |
	 (x,y+1) *--* (x+1,y+1)
	 
	 */
	void drawShaded();

	void drawShadow();

	/* this is an important methods where the time is progressed one time step for the entire cloth.
	 This includes calling satisfyConstraint() for every constraint, and calling timeStep() for all particles
	 */
	void timeStep();
	
	/* used to add gravity (or any other arbitrary vector) to all particles*/
	void addForce(const Vec3 direction);
	
	/* used to add wind forces to all particles, is added for each triangle since the final force is proportional to the triangle area as seen from the wind direction*/
	void windForce(const Vec3 direction);
	
	/* used to detect and resolve the collision of the cloth with the ball.
	 This is based on a very simples scheme where the position of each particle is simply compared to the sphere and corrected.
	 This also means that the sphere can "slip through" if the ball is small enough compared to the distance in the grid bewteen particles
	 */
	void ballCollision(const Vec3 center,const float radius );
	
	void doFrame();
};

#endif