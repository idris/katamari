#pragma once
#ifndef QUATERNION_H
#define QUATERNION_H

//-----------------------------------------------------------------------
//	File:			Quaternion.h
//	Description:	Quaternion object
//	Programmer:		Jun-Cheng Chen and Dave Mount
//	For:			CMSC 427 - Fall 2009
//
//	This is a simple class implementing some important quaternion
//	functions.
//-----------------------------------------------------------------------

#include "Vector3D.h"

//-----------------------------------------------------------------------
//	Matrix3D - simple matrix object
//
//	This matrix represents a 3x3 matrix.
//-----------------------------------------------------------------------

class Matrix3D {
public:
	bool isOrthogonal;
	double m[3][3];
};

//-----------------------------------------------------------------------
//	Quaternion
//
//	The rotation by theta degrees about axis u is represented by the
//	quaternion (s, v) where: s = sin theta/2 and v = (cos theta/2)*u.
//
//	The constructor is given the angle in degrees.
//-----------------------------------------------------------------------

class Quaternion {
public:
	double  s;							// scalar part
	Vector3D v;							// vector part
public:	
										// constructors and destructor
	Quaternion();
	Quaternion(double angle, const Vector3D& axis);
	~Quaternion();
										// assignment operators
	Quaternion& operator=(const Quaternion& q);
	Quaternion& operator+= (const Quaternion& q);
	Quaternion& operator-=(const Quaternion& q);
	Quaternion& operator*=(double scale);


	Vector3D getAxis() const;			// getters
	double getAngle() const;			// angle returned in degrees
	Matrix3D getRotationMatrix() const;

										// arithmetic operators
	void normalize();
	void setSAndV(double ss, const Vector3D& vv);
	void setAngleAndAxis(double angle, const Vector3D& axis);
	Quaternion inverse() const;
	Quaternion operator+(const Quaternion& quat) const;
	Quaternion operator-(const Quaternion& quat) const;
	Quaternion operator*(const Quaternion& quat) const;
	Vector3D rotate(const Vector3D& u) const;
	friend Quaternion operator*(const Quaternion& quat, double scalar);
	friend Quaternion operator*(double scalar, const Quaternion& quat);

										// create from s and v
	static Quaternion createFromSAndV(double ss, const Vector3D& vv);

										// derivative wrt angular veloc w
	static Quaternion derivative(const Vector3D& w, const Quaternion& q);
};

#endif
