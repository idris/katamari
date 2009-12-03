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


#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#include <GL/gl.h>
#endif
#include <math.h>
#include <vector>
#include <iostream>

#include "common.h"
#include "cloth.h"


/* Some physics constants */
#define DAMPING 0.01 // how much to damp the cloth simulation each frame
#define TIME_STEPSIZE2 0.5*0.5 // how large time step each particle takes each frame
#define CONSTRAINT_ITERATIONS 5 // how many iterations of constraint satisfaction each frame (more is rigid, less is soft)


Vec3::Vec3(float x, float y, float z)
{
	f[0] =x;
	f[1] =y;
	f[2] =z;
}

Vec3::Vec3() {}

	float Vec3::length()
	{
		return sqrt(f[0]*f[0]+f[1]*f[1]+f[2]*f[2]);
	}

	Vec3 Vec3::normalized()
	{
		float l = length();
		return Vec3(f[0]/l,f[1]/l,f[2]/l);
	}

	void Vec3::operator+= (const Vec3 &v)
	{
		f[0]+=v.f[0];
		f[1]+=v.f[1];
		f[2]+=v.f[2];
	}

	Vec3 Vec3::operator/ (const float &a)
	{
		return Vec3(f[0]/a,f[1]/a,f[2]/a);
	}

	Vec3 Vec3::operator- (const Vec3 &v)
	{
		return Vec3(f[0]-v.f[0],f[1]-v.f[1],f[2]-v.f[2]);
	}

	Vec3 Vec3::operator+ (const Vec3 &v)
	{
		return Vec3(f[0]+v.f[0],f[1]+v.f[1],f[2]+v.f[2]);
	}

	Vec3 Vec3::operator* (const float &a)
	{
		return Vec3(f[0]*a,f[1]*a,f[2]*a);
	}

	Vec3 Vec3::operator-()
	{
		return Vec3(-f[0],-f[1],-f[2]);
	}

	Vec3 Vec3::cross(const Vec3 &v)
	{
		return Vec3(f[1]*v.f[2] - f[2]*v.f[1], f[2]*v.f[0] - f[0]*v.f[2], f[0]*v.f[1] - f[1]*v.f[0]);
	}

	float Vec3::dot(const Vec3 &v)
	{
		return f[0]*v.f[0] + f[1]*v.f[1] + f[2]*v.f[2];
	}

/* The particle class represents a particle of mass that can move around in 3D space*/
	Particle::Particle(Vec3 pos) : pos(pos), old_pos(pos),acceleration(Vec3(0,0,0)), mass(1), movable(true), accumulated_normal(Vec3(0,0,0)){}
	Particle::Particle(){}

	void Particle::addForce(Vec3 f)
	{
		acceleration += f/mass;
	}

	/* This is one of the important methods, where the time is progressed a single step size (TIME_STEPSIZE)
	   The method is called by Cloth.time_step()
	   Given the equation "force = mass * acceleration" the next position is found through verlet integration*/
	void Particle::timeStep()
	{
		if(movable)
		{
			Vec3 temp = pos;
			pos = pos + (pos-old_pos)*(1.0-DAMPING) + acceleration*TIME_STEPSIZE2;
			old_pos = temp;
			acceleration = Vec3(0,0,0); // acceleration is reset since it HAS been translated into a change in position (and implicitely into velocity)	
		}
	}

	Vec3& Particle::getPos() {return pos;}

	void Particle::resetAcceleration() {acceleration = Vec3(0,0,0);}

	void Particle::offsetPos(const Vec3 v) { if(movable) pos += v;}

	void Particle::makeUnmovable() {movable = false;}

	void Particle::addToNormal(Vec3 normal)
	{
		accumulated_normal += normal.normalized();
	}

	Vec3& Particle::getNormal() { return accumulated_normal;} // notice, the normal is not unit length

	void Particle::resetNormal() {accumulated_normal = Vec3(0,0,0);}


	Constraint::Constraint(Particle *p1, Particle *p2) :  p1(p1),p2(p2)
	{
		Vec3 vec = p1->getPos()-p2->getPos();
		rest_distance = vec.length();
	}

	/* This is one of the important methods, where a single constraint between two particles p1 and p2 is solved
	the method is called by Cloth.time_step() many times per frame*/
	void Constraint::satisfyConstraint()
	{
		Vec3 p1_to_p2 = p2->getPos()-p1->getPos(); // vector from p1 to p2
		float current_distance = p1_to_p2.length(); // current distance between p1 and p2
		Vec3 correctionVector = p1_to_p2*(1 - rest_distance/current_distance); // The offset vector that could moves p1 into a distance of rest_distance to p2
		Vec3 correctionVectorHalf = correctionVector*0.5; // Lets make it half that length, so that we can move BOTH p1 and p2.
		p1->offsetPos(correctionVectorHalf); // correctionVectorHalf is pointing from p1 to p2, so the length should move p1 half the length needed to satisfy the constraint.
		p2->offsetPos(-correctionVectorHalf); // we must move p2 the negative direction of correctionVectorHalf since it points from p2 to p1, and not p1 to p2.	
	}

Particle* Cloth::getParticle(int x, int y) {return &particles[y*num_particles_width + x];}
	void Cloth::makeConstraint(Particle *p1, Particle *p2) {constraints.push_back(Constraint(p1,p2));}


	/* A private method used by drawShaded() and addWindForcesForTriangle() to retrieve the  
	normal vector of the triangle defined by the position of the particles p1, p2, and p3.
	The magnitude of the normal vector is equal to the area of the parallelogram defined by p1, p2 and p3
	*/
	Vec3 Cloth::calcTriangleNormal(Particle *p1,Particle *p2,Particle *p3)
	{
		Vec3 pos1 = p1->getPos();
		Vec3 pos2 = p2->getPos();
		Vec3 pos3 = p3->getPos();

		Vec3 v1 = pos2-pos1;
		Vec3 v2 = pos3-pos1;

		return v1.cross(v2);
	}

	/* A private method used by windForce() to calcualte the wind force for a single triangle 
	defined by p1,p2,p3*/
	void Cloth::addWindForcesForTriangle(Particle *p1,Particle *p2,Particle *p3, const Vec3 direction)
	{
		Vec3 normal = calcTriangleNormal(p1,p2,p3);
		Vec3 d = normal.normalized();
		Vec3 force = normal*(d.dot(direction));
		p1->addForce(force);
		p2->addForce(force);
		p3->addForce(force);
	}

	/* A private method used by drawShaded(), that draws a single triangle p1,p2,p3 with a color*/
	void Cloth::drawTriangle(Particle *p1, Particle *p2, Particle *p3, const Vec3 color)
	{
		glColor3fv( (GLfloat*) &color );

		glNormal3fv((GLfloat *) &(p1->getNormal().normalized() ));
		glVertex3fv((GLfloat *) &(p1->getPos() ));

		glNormal3fv((GLfloat *) &(p2->getNormal().normalized() ));
		glVertex3fv((GLfloat *) &(p2->getPos() ));

		glNormal3fv((GLfloat *) &(p3->getNormal().normalized() ));
		glVertex3fv((GLfloat *) &(p3->getPos() ));
	}

	/* This is a important constructor for the entire system of particles and constraints*/
	Cloth::Cloth(float width, float height, int num_particles_width, int num_particles_height) : num_particles_width(num_particles_width), num_particles_height(num_particles_height)
	{
		particles.resize(num_particles_width*num_particles_height); //I am essentially using this vector as an array with room for num_particles_width*num_particles_height particles

		// creating particles in a grid of particles from (0,0,0) to (width,-height,0)
		for(int x=0; x<num_particles_width; x++)
		{
			for(int y=0; y<num_particles_height; y++)
			{
				Vec3 pos = Vec3(width * (x/(float)num_particles_width),
								-height * (y/(float)num_particles_height),
								0);
				particles[y*num_particles_width+x]= Particle(pos); // insert particle in column x at y'th row
			}
		}

		// Connecting immediate neighbor particles with constraints (distance 1 and sqrt(2) in the grid)
		for(int x=0; x<num_particles_width; x++)
		{
			for(int y=0; y<num_particles_height; y++)
			{
				if (x<num_particles_width-1) makeConstraint(getParticle(x,y),getParticle(x+1,y));
				if (y<num_particles_height-1) makeConstraint(getParticle(x,y),getParticle(x,y+1));
				if (x<num_particles_width-1 && y<num_particles_height-1) makeConstraint(getParticle(x,y),getParticle(x+1,y+1));
				if (x<num_particles_width-1 && y<num_particles_height-1) makeConstraint(getParticle(x+1,y),getParticle(x,y+1));
			}
		}


		// Connecting secondary neighbors with constraints (distance 2 and sqrt(4) in the grid)
		for(int x=0; x<num_particles_width; x++)
		{
			for(int y=0; y<num_particles_height; y++)
			{
				if (x<num_particles_width-2) makeConstraint(getParticle(x,y),getParticle(x+2,y));
				if (y<num_particles_height-2) makeConstraint(getParticle(x,y),getParticle(x,y+2));
				if (x<num_particles_width-2 && y<num_particles_height-2) makeConstraint(getParticle(x,y),getParticle(x+2,y+2));
				if (x<num_particles_width-2 && y<num_particles_height-2) makeConstraint(getParticle(x+2,y),getParticle(x,y+2));			}
		}


		// making the upper left most three and right most three particles unmovable
		for(int i=0;i<3; i++)
		{
			getParticle(0+i ,0)->offsetPos(Vec3(0.5,0.0,0.0)); // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
			getParticle(0+i ,0)->makeUnmovable(); 

			getParticle(0+i ,0)->offsetPos(Vec3(-0.5,0.0,0.0)); // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
			getParticle(num_particles_width-1-i ,0)->makeUnmovable();
		}
	}

	/* drawing the cloth as a smooth shaded (and colored according to column) OpenGL triangular mesh
	Called from the display() method
	The cloth is seen as consisting of triangles for four particles in the grid as follows:

	(x,y)   *--* (x+1,y)
	        | /|
	        |/ |
	(x,y+1) *--* (x+1,y+1)

	*/
	void Cloth::drawShaded()
	{
		// reset normals (which where written to last frame)
		std::vector<Particle>::iterator particle;
		for(particle = particles.begin(); particle != particles.end(); particle++)
		{
			(*particle).resetNormal();
		}

		//create smooth per particle normals by adding up all the (hard) triangle normals that each particle is part of
		for(int x = 0; x<num_particles_width-1; x++)
		{
			for(int y=0; y<num_particles_height-1; y++)
			{
				Vec3 normal = calcTriangleNormal(getParticle(x+1,y),getParticle(x,y),getParticle(x,y+1));
				getParticle(x+1,y)->addToNormal(normal);
				getParticle(x,y)->addToNormal(normal);
				getParticle(x,y+1)->addToNormal(normal);

				normal = calcTriangleNormal(getParticle(x+1,y+1),getParticle(x+1,y),getParticle(x,y+1));
				getParticle(x+1,y+1)->addToNormal(normal);
				getParticle(x+1,y)->addToNormal(normal);
				getParticle(x,y+1)->addToNormal(normal);
			}
		}

		glBegin(GL_TRIANGLES);
		for(int x = 0; x<num_particles_width-1; x++)
		{
			for(int y=0; y<num_particles_height-1; y++)
			{
				Vec3 color(0,0,0);
				if (x%2) // red and white color is interleaved according to which column number
					color = Vec3(0.6f,0.2f,0.2f);
				else
					color = Vec3(1.0f,1.0f,1.0f);

				drawTriangle(getParticle(x+1,y),getParticle(x,y),getParticle(x,y+1),color);
				drawTriangle(getParticle(x+1,y+1),getParticle(x+1,y),getParticle(x,y+1),color);
			}
		}
		glEnd();
	}

void Cloth::drawShadow()
{
	Vec3 color = Vec3(0.3f, 0.1, 0.1);
	// reset normals (which where written to last frame)
	std::vector<Particle>::iterator particle;
	for(particle = particles.begin(); particle != particles.end(); particle++)
	{
		(*particle).resetNormal();
	}
	
	//create smooth per particle normals by adding up all the (hard) triangle normals that each particle is part of
	for(int x = 0; x<num_particles_width-1; x++)
	{
		for(int y=0; y<num_particles_height-1; y++)
		{
			Vec3 normal = calcTriangleNormal(getParticle(x+1,y),getParticle(x,y),getParticle(x,y+1));
			getParticle(x+1,y)->addToNormal(normal);
			getParticle(x,y)->addToNormal(normal);
			getParticle(x,y+1)->addToNormal(normal);
			
			normal = calcTriangleNormal(getParticle(x+1,y+1),getParticle(x+1,y),getParticle(x,y+1));
			getParticle(x+1,y+1)->addToNormal(normal);
			getParticle(x+1,y)->addToNormal(normal);
			getParticle(x,y+1)->addToNormal(normal);
		}
	}
	
	glBegin(GL_TRIANGLES);
	for(int x = 0; x<num_particles_width-1; x++)
	{
		for(int y=0; y<num_particles_height-1; y++)
		{
			drawTriangle(getParticle(x+1,y),getParticle(x,y),getParticle(x,y+1),color);
			drawTriangle(getParticle(x+1,y+1),getParticle(x+1,y),getParticle(x,y+1),color);
		}
	}
	glEnd();
}

	/* this is an important methods where the time is progressed one time step for the entire cloth.
	This includes calling satisfyConstraint() for every constraint, and calling timeStep() for all particles
	*/
	void Cloth::timeStep()
	{
		std::vector<Constraint>::iterator constraint;
		for(int i=0; i<CONSTRAINT_ITERATIONS; i++) // iterate over all constraints several times
		{
			for(constraint = constraints.begin(); constraint != constraints.end(); constraint++ )
			{
				(*constraint).satisfyConstraint(); // satisfy constraint.
			}
		}

		std::vector<Particle>::iterator particle;
		for(particle = particles.begin(); particle != particles.end(); particle++)
		{
			(*particle).timeStep(); // calculate the position of each particle at the next time step.
		}
	}

	/* used to add gravity (or any other arbitrary vector) to all particles*/
	void Cloth::addForce(const Vec3 direction)
	{
		std::vector<Particle>::iterator particle;
		for(particle = particles.begin(); particle != particles.end(); particle++)
		{
			(*particle).addForce(direction); // add the forces to each particle
		}

	}

	/* used to add wind forces to all particles, is added for each triangle since the final force is proportional to the triangle area as seen from the wind direction*/
	void Cloth::windForce(const Vec3 direction)
	{
		for(int x = 0; x<num_particles_width-1; x++)
		{
			for(int y=0; y<num_particles_height-1; y++)
			{
				addWindForcesForTriangle(getParticle(x+1,y),getParticle(x,y),getParticle(x,y+1),direction);
				addWindForcesForTriangle(getParticle(x+1,y+1),getParticle(x+1,y),getParticle(x,y+1),direction);
			}
		}
	}

	/* used to detect and resolve the collision of the cloth with the ball.
	This is based on a very simples scheme where the position of each particle is simply compared to the sphere and corrected.
	This also means that the sphere can "slip through" if the ball is small enough compared to the distance in the grid bewteen particles
	*/
	void Cloth::ballCollision(const Vec3 center,const float radius )
	{
		std::vector<Particle>::iterator particle;
		for(particle = particles.begin(); particle != particles.end(); particle++)
		{
			Vec3 v = (*particle).getPos()-center;
			float l = v.length();
			if ( v.length() < radius) // if the particle is inside the ball
			{
				(*particle).offsetPos(v.normalized()*(radius-l)); // project the particle to the surface of the ball
			}
		}
	}

	void Cloth::doFrame()
	{

	}


/***** Above are definition of classes; Vec3, Particle, Constraint, and Cloth *****/




// Just below are three global variables holding the actual animated stuff; Cloth and Ball 
Cloth cloth1(14,10,55,45); // one Cloth object of the Cloth class
Vec3 ball_pos(7,-5,0); // the center of our one ball
float ball_radius = 2; // the radius of our one ball



/***** Below are functions Init(), display(), reshape(), keyboard(), arrow_keys(), main() *****/

/* This is where all the standard Glut/OpenGL stuff is, and where the methods of Cloth are called; 
addForce(), windForce(), timeStep(), ballCollision(), and drawShaded()*/

/*
void init(void)
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.2f, 0.2f, 0.4f, 0.5f);				
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_COLOR_MATERIAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat lightPos[4] = {-1.0,1.0,0.5,0.0};
	glLightfv(GL_LIGHT0,GL_POSITION,(GLfloat *) &lightPos);

	glEnable(GL_LIGHT1);

	GLfloat lightAmbient1[4] = {0.0,0.0,0.0,0.0};
	GLfloat lightPos1[4] = {1.0,0.0,-0.2,0.0};
	GLfloat lightDiffuse1[4] = {0.5,0.5,0.3,0.0};

	glLightfv(GL_LIGHT1,GL_POSITION,(GLfloat *) &lightPos1);
	glLightfv(GL_LIGHT1,GL_AMBIENT,(GLfloat *) &lightAmbient1);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,(GLfloat *) &lightDiffuse1);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
}
*/

float ball_time = 0; // counter for used to calculate the z position of the ball below

/* display method called each frame*/
/*void display(void)
{
	// calculating positions

	ball_time++;
	ball_pos.f[2] = cos(ball_time/50.0)*7;

	cloth1.addForce(Vec3(0,-0.2,0)*TIME_STEPSIZE2); // add gravity each frame, pointing down
	cloth1.windForce(Vec3(0.5,0,0.2)*TIME_STEPSIZE2); // generate some wind each frame
	cloth1.timeStep(); // calculate the particle positions of the next frame
	cloth1.ballCollision(ball_pos,ball_radius); // resolve collision with the ball



	// drawing

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glDisable(GL_LIGHTING); // drawing some smooth shaded background - because I like it ;)
	glBegin(GL_POLYGON);
	glColor3f(0.8f,0.8f,1.0f);
	glVertex3f(-200.0f,-100.0f,-100.0f);
	glVertex3f(200.0f,-100.0f,-100.0f);
	glColor3f(0.4f,0.4f,0.8f);	
	glVertex3f(200.0f,100.0f,-100.0f);
	glVertex3f(-200.0f,100.0f,-100.0f);
	glEnd();
	glEnable(GL_LIGHTING);

	glTranslatef(-6.5,6,-9.0f); // move camera out and center on the cloth
	glRotatef(25,0,1,0); // rotate a bit to see the cloth from the side
	cloth1.drawShaded(); // finally draw the cloth with smooth shading

	glPushMatrix(); // to draw the ball we use glutSolidSphere, and need to draw the sphere at the position of the ball
	glTranslatef(ball_pos.f[0],ball_pos.f[1],ball_pos.f[2]); // hence the translation of the sphere onto the ball position
	glColor3f(0.4f,0.8f,0.5f);
	glutSolidSphere(ball_radius-0.1,50,50); // draw the ball, but with a slightly lower radius, otherwise we could get ugly visual artifacts of cloth penetrating the ball slightly
	glPopMatrix();

	glutSwapBuffers();
	glutPostRedisplay();
}
*/

/*
int main ( int argc, char** argv ) 
{
	glutInit( &argc, argv );

	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE ); 
	glutInitWindowSize(1280, 720 ); 

	glutCreateWindow( "Cloth Tutorial from cg.alexandra.dk" );
	init();
	glutDisplayFunc(display);  

	glutMainLoop();
}*/