/*
 * math.h
 *
 *  Created on: Mar 2, 2015
 *      Author: skim
 */

#ifndef MATH_H_
#define MATH_H_

#include "common.h"

typedef float vec3_t[3];
typedef float vec_t;
#define MAXSTUDIOBONES		128		// total bones actually used
#define BONE_USED_BY_HITBOX			0x00000100	// bone (or child) is used by a hit box
#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI) )
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI / 180.f) )

struct Vector
{
	Vector() : x(0), y(0), z(0){}
	Vector(float _x, float _y, float _z) : x(_x), y(_y), z(_z){}
	float x, y, z;

	float LengthSqr(void) const
	{
		return (x*x + y*y + z*z);
	}
	float& operator[](int i) const
	{
		return ((float*)this)[i];
	}

	Vector&	operator+=(const Vector &v);
	Vector&	operator-=(const Vector &v);
	Vector&	operator*=(const Vector &v);
	Vector&	operator*=(float s);
	Vector&	operator+=(float fl) ; ///< broadcast add
	Vector&	operator-=(float fl) ; ///< broadcast sub
	Vector	operator-(void) const;
	Vector	operator+(const Vector& v) const;
	Vector	operator-(const Vector& v) const;
	Vector	operator*(const Vector& v) const;
	Vector	operator*(float fl) const;
};

struct Vector2D
{
	float x, y;
	Vector2D(float _x, float _y) : x(_x), y(_y) {}
	Vector2D() : x(0), y(0) {}
};

struct QAngle : public Vector
{};

class VMatrix
{
public:
	vec_t		m[4][4];
	inline float* operator[](int i)
	{
		return m[i];
	}
	inline const float* operator[](int i) const
	{
		return m[i];
	}

};
struct matrix3x4_t
{
	matrix3x4_t() {}
	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23 )
	{
		m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	void Init( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector &vecOrigin )
	{
		m_flMatVal[0][0] = xAxis.x; m_flMatVal[0][1] = yAxis.x; m_flMatVal[0][2] = zAxis.x; m_flMatVal[0][3] = vecOrigin.x;
		m_flMatVal[1][0] = xAxis.y; m_flMatVal[1][1] = yAxis.y; m_flMatVal[1][2] = zAxis.y; m_flMatVal[1][3] = vecOrigin.y;
		m_flMatVal[2][0] = xAxis.z; m_flMatVal[2][1] = yAxis.z; m_flMatVal[2][2] = zAxis.z; m_flMatVal[2][3] = vecOrigin.z;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	matrix3x4_t( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector &vecOrigin )
	{
		Init( xAxis, yAxis, zAxis, vecOrigin );
	}

	float *operator[]( int i )				{ return m_flMatVal[i]; }
	const float *operator[]( int i ) const	{ return m_flMatVal[i]; }
	float *Base()							{ return &m_flMatVal[0][0]; }
	const float *Base() const				{ return &m_flMatVal[0][0]; }

	float m_flMatVal[3][4];
};

Vector AngleVectors(const QAngle& angle);
void AngleVectors(const QAngle& angles, Vector*, Vector*, Vector*);
void VectorAngles( const Vector& forward, QAngle &angles );
void VectorAdd( const Vector& a, const Vector& b, Vector& c );
void VectorSubtract( const Vector& a, const Vector& b, Vector& c );
void VectorClear(Vector& a);
void VectorMultiply(Vector& a, float b);
void VectorMultiply( const Vector& a, vec_t b, Vector& c );
void VectorMultiply( const Vector& a, const Vector& b, Vector& c );
void VectorTransform (const float *in1, const matrix3x4_t& in2, float *out);
float VectorNormalize(Vector& a);
vec_t DotProduct(const Vector& a, const Vector& b);
vec_t DotProduct(const vec_t *v1, const vec_t *v2);
void MatrixAngles( const matrix3x4_t& matrix, float *angles );

// Math routines done in optimized assembly math package routines
void inline SinCos( float radians, float *sine, float *cosine )
{
	double __cosr, __sinr;
	__asm ("fsincos" : "=t" (__cosr), "=u" (__sinr) : "0" (radians));

  	*sine = __sinr;
  	*cosine = __cosr;
}
inline void VectorTransform (const Vector& in1, const matrix3x4_t &in2, Vector &out)
{
	VectorTransform( &in1.x, in2, &out.x );
}
inline void MatrixAngles( const matrix3x4_t &matrix, QAngle &angles )
{
	MatrixAngles( matrix, &angles.x );
}

#endif /* MATH_H_ */
