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
	float sy = sinf(a);
	float cy = cosf(a);

	a = angle.x * (M_PI*2 / 360);
	float sp = sinf(a);
	float cp = cosf(a);

	return (Vector){cp*cy, cp*sy, -sp};
}
void AngleVectors( const QAngle &angles, Vector *forward, Vector *right, Vector *up )
{
	float sr, sp, sy, cr, cp, cy;

	SinCos( DEG2RAD( angles[1] ), &sy, &cy );
	SinCos( DEG2RAD( angles[0] ), &sp, &cp );
	SinCos( DEG2RAD( angles[2] ), &sr, &cr );

	if (forward)
	{
		forward->x = cp*cy;
		forward->y = cp*sy;
		forward->z = -sp;
	}

	if (right)
	{
		right->x = (-1*sr*sp*cy+-1*cr*-sy);
		right->y = (-1*sr*sp*sy+-1*cr*cy);
		right->z = -1*sr*cp;
	}

	if (up)
	{
		up->x = (cr*sp*cy+-sr*-sy);
		up->y = (cr*sp*sy+-sr*cy);
		up->z = cr*cp;
	}
}
void VectorAngles( const Vector& forward, QAngle &angles )
{
	float	tmp, yaw, pitch;

	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = (atan2f(forward[1], forward[0]) * 180 / M_PI);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrtf(forward[0]*forward[0] + forward[1]*forward[1]);
		pitch = (atan2f(-forward[2], tmp) * 180 / M_PI);
		if (pitch < 0)
			pitch += 360;
	}

	angles.x = pitch;
	angles.y = yaw;
	angles.z = 0;
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
float VectorNormalize(Vector& v)
{
	float length, ilength;

	length = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
	length = sqrtf(length);

	if (length > 0.000001)
	{
		ilength = 1/length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}
	return length;
}
vec_t DotProduct(const Vector& a, const Vector& b)
{
	return( a.x*b.x + a.y*b.y + a.z*b.z );
}
vec_t DotProduct(const vec_t *v1, const vec_t *v2)
{
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

void MatrixAngles( const matrix3x4_t& matrix, float *angles )
{
	float forward[3];
	float left[3];
	float up[3];

	//
	// Extract the basis vectors from the matrix. Since we only need the Z
	// component of the up vector, we don't get X and Y.
	//
	forward[0] = matrix[0][0];
	forward[1] = matrix[1][0];
	forward[2] = matrix[2][0];
	left[0] = matrix[0][1];
	left[1] = matrix[1][1];
	left[2] = matrix[2][1];
	up[2] = matrix[2][2];

	float xyDist = sqrtf( forward[0] * forward[0] + forward[1] * forward[1] );

	// enough here to get angles?
	if ( xyDist > 0.001f )
	{
		// (yaw)	y = ATAN( forward.y, forward.x );		-- in our space, forward is the X axis
		angles[1] = RAD2DEG( atan2f( forward[1], forward[0] ) );

		// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
		angles[0] = RAD2DEG( atan2f( -forward[2], xyDist ) );

		// (roll)	z = ATAN( left.z, up.z );
		angles[2] = RAD2DEG( atan2f( left[2], up[2] ) );
	}
	else	// forward is mostly Z, gimbal lock-
	{
		// (yaw)	y = ATAN( -left.x, left.y );			-- forward is mostly z, so use right for yaw
		angles[1] = RAD2DEG( atan2f( -left[0], left[1] ) );

		// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
		angles[0] = RAD2DEG( atan2f( -forward[2], xyDist ) );

		// Assume no roll in this case as one degree of freedom has been lost (i.e. yaw == roll)
		angles[2] = 0;
	}
}
