//-----------------------------------------------------------------------
//	File:			Vector3D.cpp
//	Description:	Vectors and Points in 3D
//	Programmer:		Dave Mount
//	For:			CMSC 427 - Fall 2009
//-----------------------------------------------------------------------

#include "Vector3D.h"

using namespace std;					// make std:: accessible

//----------------------------------------------------------------------
// Constructors and Destructors
//----------------------------------------------------------------------

Vector3D::Vector3D(): x(0), y(0), z(0) {}
Vector3D::Vector3D(double _x, double _y, double _z): x(_x), y(_y), z(_z) {}
Vector3D::Vector3D(const Vector3D& copy): x(copy.x), y(copy.y), z(copy.z) {}
Vector3D::~Vector3D() {}

//----------------------------------------------------------------------
// Assignment operators
//----------------------------------------------------------------------

Vector3D& Vector3D::operator=(const Vector3D& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

Vector3D& Vector3D::operator+=(const Vector3D& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vector3D& Vector3D::operator*=(double s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

//----------------------------------------------------------------------
// Arithmetic operators
//----------------------------------------------------------------------

Vector3D operator+(const Vector3D& v1, const Vector3D& v2)
{
	Vector3D res(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z);
	return res;
}

Vector3D operator-(const Vector3D& v1, const Vector3D& v2)
{
	Vector3D res(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z);
	return res;
}

Vector3D operator*(double s, const Vector3D& v)
{
	Vector3D res(s*v.x, s*v.y, s*v.z);
	return res;
}

Vector3D operator*(const Vector3D& v, double s)
{
	return s * v;
}

//----------------------------------------------------------------------
// Additional methods
//----------------------------------------------------------------------

double Vector3D::length() const
{
	return sqrt(x*x + y*y + z*z);
}

void Vector3D::clamp(double min, double max)
{
    double len = length();
	if (max < GEOM_INF && len > max) {          // too long?
        x *= (max/len);
		y *= (max/len);
		z *= (max/len);
    }
    else if (min > 0 && len < min) {            // too short?
        x *= (min/len);
		y *= (min/len);
		z *= (min/len);
    }
}

void Vector3D::normalize()				// normalize to unit length
{
	double len = length();
	if (len < GEOM_EPS) return;
	x /= len;
	y /= len;
	z /= len;
}

										// functional form 
Vector3D Vector3D::normalize(const Vector3D& v)
{
	double len = v.length();
	if (len < GEOM_EPS) return v;
	else return (1/len) * v;
}

//----------------------------------------------------------------------
// Additional utilities
//----------------------------------------------------------------------

double Vector3D::dist(const Vector3D& v1, const Vector3D& v2)
{
	return sqrt(
		  (v1.x - v2.x)*(v1.x - v2.x)
		+ (v1.y - v2.y)*(v1.y - v2.y) 
		+ (v1.z - v2.z)*(v1.z - v2.z));
}

double Vector3D::dot(const Vector3D& v1, const Vector3D& v2)
{
	return  (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}
										// cross product
Vector3D Vector3D::cross(const Vector3D& v1, const Vector3D& v2)
{
	return Vector3D(
		v1.y*v2.z - v1.z*v2.y,
		v1.z*v2.x - v1.x*v2.z,
		v1.x*v2.y - v1.y*v2.x);
}

//----------------------------------------------------------------------
//	Projection:
//		project - project v1 orthogonally onto v2
//		orthog - orthogonal complement of v1 wrt v2
//
//		Thus: project(v1, v2) + orthog(v1, v2) == v1
//----------------------------------------------------------------------

Vector3D Vector3D::project(const Vector3D& v1, const Vector3D& v2)
{
	return Vector3D((Vector3D::dot(v1, v2)*(1/Vector3D::dot(v2, v2))) * v2);
}

Vector3D Vector3D::orthog(const Vector3D& v1, const Vector3D& v2)
{
	return v1 - Vector3D::project(v1, v2);
}


//----------------------------------------------------------------------
// Converts Cartesian to spherical coords
//	from (x,y,z) to (radius, theta, phi) in radians, where theta gives
//	the longitude and phi gives the latitude (start at 0 for north pole)
//----------------------------------------------------------------------

Vector3D Vector3D::convertCartesianToSpherical(const Vector3D& v)
{
	double lenXY = sqrt(v.x*v.x + v.y*v.y);

	Vector3D spherical;
	spherical.x = v.length();
	spherical.y = atan2(v.y, v.x);	// theta
	spherical.z = -atan2(v.z, lenXY);	// phi

	return spherical;
}

//----------------------------------------------------------------------
// Converts spherical to Cartesian
// 	from (rho, theta, phi) in radians to (x,y,z)
//----------------------------------------------------------------------

Vector3D Vector3D::convertSphericalToCartesian(const Vector3D& v)
{
	Vector3D cartesian;
	cartesian.x = v.x * cos(v.y) * sin(v.z);
	cartesian.y = v.x * sin(v.y) * sin(v.z);
	cartesian.z = v.x * cos(v.z);

	return cartesian;
}

//----------------------------------------------------------------------
//	Common vectors
//----------------------------------------------------------------------
Vector3D Vector3D::zero() {	return Vector3D(0, 0, 0); }
Vector3D Vector3D::yUnit() { return Vector3D(1, 0, 0); }
Vector3D Vector3D::xUnit() { return Vector3D(0, 1, 0); }
Vector3D Vector3D::zUnit() { return Vector3D(0, 0, 1); }

//----------------------------------------------------------------------
//	Output operator
//----------------------------------------------------------------------
ostream& operator<<(ostream& out, const Vector3D& v) {
	return out << "[" << v.x << ", " << v.y << ", " << v.z << "]";
}
