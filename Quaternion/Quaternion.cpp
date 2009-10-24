//-----------------------------------------------------------------------
//	File:			Quaternion.cpp
//	Description:	Quaternion object
//	Programmer:		Dave Mount
//	For:			CMSC 427 - Fall 2009
//
//	Quaternion methods
//-----------------------------------------------------------------------

#include "Quaternion.h"

//-----------------------------------------------------------------------
//	A quaternion is represented by a quadruple (s, v.x, v.y, v.z), where
//	s = sin (theta/2) and v = (cos (theta/2))*u, where u is a unit
//	rotation axis.
//
//	Beware: The constructor is given (angle, rotation-axis), which is
//	different from (s, v). To create a Quaternion from (s,v), use the
//	createFromSAndV() function.
//-----------------------------------------------------------------------

Quaternion::Quaternion() 
{
	setAngleAndAxis(0, Vector3D(1, 0, 0));
}

Quaternion::Quaternion(double angle, const Vector3D& axis) 
{
	setAngleAndAxis(angle, axis);
}

Quaternion::~Quaternion() { }

//-----------------------------------------------------------------------
//	Assignment operators
//-----------------------------------------------------------------------

Quaternion& Quaternion::operator=(const Quaternion& q)
{
	s = q.s;  v = q.v;  return(*this);
}

Quaternion& Quaternion::operator+= (const Quaternion& q)
{
	s += q.s; v += q.v;  return(*this);
}

Quaternion& Quaternion::operator-=(const Quaternion& q)
{
	s -= q.s; v -= q.v;  return(*this);
}

Quaternion& Quaternion::operator*=(double scale)
{
	s *= scale; v *= scale;  return(*this);
}

//-----------------------------------------------------------------------
//	Getters
//-----------------------------------------------------------------------

Vector3D Quaternion::getAxis() const
{
	return Vector3D::normalize(v);
}

double Quaternion::getAngle() const
{
	return acos(s) * 360 / M_PI;
}

Matrix3D Quaternion::getRotationMatrix() const 
{
	Quaternion tmp = *this;
	tmp.normalize();						// first normalize

	double vxSquare = 2 * tmp.v.x * tmp.v.x;
	double vySquare = 2 * tmp.v.y * tmp.v.y;
	double vzSquare = 2 * tmp.v.z * tmp.v.z;
	double vx_vy = 2 * tmp.v.x * tmp.v.y;
	double vx_vz = 2 * tmp.v.x * tmp.v.z;
	double vy_vz = 2 * tmp.v.y * tmp.v.z;
	double v_x = 2 * s * tmp.v.x;
	double v_y = 2 * s * tmp.v.y;
	double v_z = 2 * s * tmp.v.z;

	Matrix3D mat;
	mat.m[0][0] = 1 - vySquare -  vzSquare;
	mat.m[0][1] = vx_vy + v_z;
	mat.m[0][2] = vx_vz - v_y;
	mat.m[1][0] = vx_vy - v_z; 
	mat.m[1][1] = 1 - vxSquare - vzSquare;
	mat.m[1][2] = vy_vz + v_x;
	mat.m[2][0] = vx_vz + v_y;
	mat.m[2][1] = vy_vz - v_x;
	mat.m[2][2] = 1 - vxSquare - vySquare;

	return mat;
}

//-----------------------------------------------------------------------
//	Arithmetic operators
//		setSAndV sets basic quaternion members (s,v)
//		setAngleAndAxis sets angle and rotation axis
//-----------------------------------------------------------------------

void Quaternion::normalize() 
{
	double length = sqrt(v.x*v.x + v.y*v.y + v.z*v.z + s*s);
	v.x /= length; 
	v.y /= length; 
	v.z /= length;
	s /= length;
}

void Quaternion::setSAndV(double ss, const Vector3D& vv) 
{
	s = ss;
	v = vv;
}

void Quaternion::setAngleAndAxis(double angle, const Vector3D& axis) 
{
	double halfTheta = angle / 360 * M_PI;
	s = cos(halfTheta);
	v = sin(halfTheta) * axis;
}

Quaternion Quaternion::inverse() const
{
	double sqLength = v.x*v.x + v.y*v.y + v.z*v.z + s*s;
	return createFromSAndV(s/sqLength, (-1/sqLength)*v);
}

Quaternion Quaternion::operator+(const Quaternion& quat) const
{
	return createFromSAndV(s + quat.s, v + quat.v);
}

Quaternion Quaternion::operator-(const Quaternion& quat) const
{
	return createFromSAndV(s - quat.s, v - quat.v);
}

Quaternion Quaternion::operator*(const Quaternion& q) const
{
	return createFromSAndV(
			s * q.s - Vector3D::dot(v, q.v),
			s*q.v + q.s*v + Vector3D::cross(v, q.v) );
}

Vector3D Quaternion::rotate(const Vector3D& u) const
{
	Quaternion uu = createFromSAndV(0, u);
	Quaternion result = uu * inverse();
	result = *this * result;
	return result.v;
}

Quaternion operator*(const Quaternion& quat, double scalar)
{
	return Quaternion::createFromSAndV(scalar * quat.s, scalar * quat.v);
}

Quaternion operator*(double scalar, const Quaternion& quat)
{
	return Quaternion::createFromSAndV(scalar * quat.s, scalar * quat.v);
}

Quaternion Quaternion::createFromSAndV(double ss, const Vector3D& vv)
{
	Quaternion tmp;
	tmp.setSAndV(ss, vv);
	return tmp;
}

Quaternion Quaternion::derivative(const Vector3D& w, const Quaternion& q)
{
										// quaternion (0, w/2)
	Quaternion ww = createFromSAndV(0, 0.5 * w);	
	return ww * q;						// derivative is product
}
