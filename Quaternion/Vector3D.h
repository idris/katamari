#pragma once
#ifndef VECTOR3D_H
#define VECTOR3D_H

//-----------------------------------------------------------------------
//	File:			Vector3D.h
//	Description:	Vectors and Points in 3D
//	Programmer:		Dave Mount
//	For:			CMSC 427 - Fall 2009
//-----------------------------------------------------------------------

#define _USE_MATH_DEFINES				// make M_PI available

#include <cstdlib>						// standard C definitions
#include <cmath>						// math includes
#include <iostream>						// C++ I/O

//-----------------------------------------------------------------------
// Useful constants
//-----------------------------------------------------------------------
const double GEOM_EPS = 1.0E-6;			// small number for float compare
const double GEOM_INF = 1.0E+30;		// a big number

//-----------------------------------------------------------------------
//	Vector3D - 3-dimensional points and vectors
//
//	This class represents a 3D vector with overloaded operators to
//	perform simple arithmetic on the vector. The class fields are
//	public for efficiency and ease of use. 
//
//	For simplicity, rather than define a separate Point3D class, we
//	simply alias Point3D to Vector3D.
//----------------------------------------------------------------------

class Vector3D;
typedef Vector3D Point3D;				// Point is alias for Vector

class Vector3D {
public:									// public members for easy access
	double x, y, z;						// coordinates
public:
	Vector3D();							// constructors and destructors
	Vector3D(double x, double y, double z);
	Vector3D(const Vector3D& v);
	~Vector3D();
										// assignment operators
	Vector3D& operator=(const Vector3D& v);
	Vector3D& operator+= (const Vector3D& v);
	Vector3D& operator-=(const Vector3D& v);
	Vector3D& operator*=(double s);

	double length() const;				// additional methods
	void clamp(double min, double max);
	void normalize();
	static Vector3D normalize(const Vector3D& v);

										// additional utilities
	static double dist(const Vector3D& v1, const Vector3D& v2);
	static double dot(const Vector3D& v1, const Vector3D& v2);
	static Vector3D cross(const Vector3D& v1, const Vector3D& v2);
	static Vector3D project(const Vector3D& v1, const Vector3D& v2);
	static Vector3D orthog(const Vector3D& v1, const Vector3D& v2);
	static Vector3D convertCartesianToSpherical(const Vector3D& v);
	static Vector3D convertSphericalToCartesian(const Vector3D& v);

	static Vector3D zero();				// commonly used vectors
	static Vector3D zUnit();
	static Vector3D yUnit();
	static Vector3D xUnit();

										// arithmetic operators
	friend Vector3D operator+(const Vector3D& v1, const Vector3D& v2);
	friend Vector3D operator-(const Vector3D& v1, const Vector3D& v2);
	friend Vector3D operator*(double s, const Vector3D& v);
	friend Vector3D operator*(const Vector3D& v, double s);
};

//----------------------------------------------------------------------
//	Output operator
//----------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const Vector3D& v);

#endif
