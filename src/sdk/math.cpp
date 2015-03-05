/*
 * math.cpp
 *
 *  Created on: Mar 5, 2015
 *      Author: skim
 */

#include "math.h"

#include <math.h>

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
void VectorClear(Vector& a)
{
	a.x = a.y = a.z = 0;
}
void VectorMultiply(Vector& a, float b)
{
	a.x *= b;
	a.y *= b;
	a.z *= b;
}
