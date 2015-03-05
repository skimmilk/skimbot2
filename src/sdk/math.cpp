/*
 * math.cpp
 *
 *  Created on: Mar 5, 2015
 *      Author: skim
 */

#include "math.h"

#include <math.h>

Vector& Vector::operator+=(const Vector& v)
{
	x+=v.x; y+=v.y; z += v.z;
	return *this;
}
Vector& Vector::operator-=(const Vector& v)
{
	x-=v.x; y-=v.y; z -= v.z;
	return *this;
}
Vector& Vector::operator*=(float fl)
{
	x *= fl;
	y *= fl;
	z *= fl;
	return *this;
}
Vector& Vector::operator*=(const Vector& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;
}
// this ought to be an opcode.
Vector&	Vector::operator+=(float fl)
{
	x += fl;
	y += fl;
	z += fl;
	return *this;
}
Vector&	Vector::operator-=(float fl)
{
	x -= fl;
	y -= fl;
	z -= fl;
	return *this;
}
Vector Vector::operator-(void) const
{
	return Vector(-x,-y,-z);
}
Vector Vector::operator+(const Vector& v) const
{
	Vector res;
	VectorAdd( *this, v, res );
	return res;
}
Vector Vector::operator-(const Vector& v) const
{
	Vector res;
	VectorSubtract( *this, v, res );
	return res;
}
Vector Vector::operator*(float fl) const
{
	Vector res;
	VectorMultiply( *this, fl, res );
	return res;
}
Vector Vector::operator*(const Vector& v) const
{
	Vector res;
	VectorMultiply( *this, v, res );
	return res;
}
Vector operator*(float fl, const Vector& v)
{
	return v * fl;
}

Vector AngleVectors(const QAngle& angle)
{
	// Convert pitch, yaw, roll to radians and calculate normalized vector
	float a = angle.y * (M_PI*2 / 360);
	float sy = sin(a);
	float cy = cos(a);

	a = angle.x * (M_PI*2 / 360);
	float sp = sin(a);
	float cp = cos(a);

	return (Vector){cp*cy, cp*sy, -sp};
}
void VectorAdd( const Vector& a, const Vector& b, Vector& c )
{
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
}
void VectorSubtract( const Vector& a, const Vector& b, Vector& c )
{
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}
void VectorMultiply(Vector& a, float b)
{
	a.x *= b;
	a.y *= b;
	a.z *= b;
}
void VectorMultiply( const Vector& a, vec_t b, Vector& c )
{
	c.x = a.x * b;
	c.y = a.y * b;
	c.z = a.z * b;
}
void VectorMultiply( const Vector& a, const Vector& b, Vector& c )
{
	c.x = a.x * b.x;
	c.y = a.y * b.y;
	c.z = a.z * b.z;
}
// transform in1 by the matrix in2
void VectorTransform (const float *in1, const matrix3x4_t& in2, float *out)
{
	out[0] = DotProduct(in1, in2[0]) + in2[0][3];
	out[1] = DotProduct(in1, in2[1]) + in2[1][3];
	out[2] = DotProduct(in1, in2[2]) + in2[2][3];
}
vec_t DotProduct(const Vector& a, const Vector& b)
{
	return( a.x*b.x + a.y*b.y + a.z*b.z );
}
vec_t DotProduct(const vec_t *v1, const vec_t *v2)
{
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}
