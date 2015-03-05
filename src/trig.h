/*
 * trig.h
 *
 *  Created on: Mar 2, 2015
 *      Author: skim
 *
 * Purpose: utility that helps with setting angles because I suck at trigonometry
 */

#ifndef TRIG_H_
#define TRIG_H_

#include "sdk/usercmd.h"
#include "sdk/common.h"

namespace skim
{

class trig
{
public:
	// Offset your movement heading by amount
	// Angles are NOT expressed in radians
	static void moveangle(float& forward, float& side, const float& angle, const float& y);
	static void moveangle(CUserCmd* cmd, const float& y);

	// Difference between two angles
	static float diffangle(float, float);

	// Set aim towards position in space
	// If move is true, cmd's movement will be modified to keep moving forward
	static void aim(CUserCmd* cmd, const Vector& eyes, const Vector& at, bool move);
	// Set angle to aim directly at point in space
	static void aim(QAngle& ang, const Vector& from, const Vector& to);
};

} /* namespace skim */

#endif /* TRIG_H_ */
