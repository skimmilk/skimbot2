/*
 * math.h
 *
 *  Created on: Mar 2, 2015
 *      Author: skim
 */

#ifndef MATH_H_
#define MATH_H_

#include <math.h>
#include "common.h"

inline Vector AngleVectors(const QAngle& angle)
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

#endif /* MATH_H_ */
