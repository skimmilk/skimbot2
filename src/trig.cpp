/*
 * trig.cpp
 *
 *  Created on: Mar 2, 2015
 *      Author: skim
 */

#include "trig.h"

#include <math.h>

namespace skim
{

void trig::moveangle(float& forward, float& side, const float& angle, const float& y)
{
	// fixen
	side *= -1.f;

	// The angle of our movement
	float angle_move = atan2f(side, forward) * 180.f / M_PI;
	float speed = hypotf(forward, side);
	// Desired movement angle
	float angle_newmove = angle - angle_move - y;

	// newx = r cos angle etc...
	// Attempt to correct floating point errors in advance
	//   by enforcing trigonometric identities
	// +-90 +-0.1
	if (fabsf(fabsf(angle_newmove) - 90.f) < .1)
		forward = 0;
	else
		forward = speed * cos(angle_newmove / 180.f * M_PI);

	if (fabsf(angle - 180.f) < .1 && fabsf(side) < .1)
		side = 0;
	else
		side = speed * sin(angle_newmove / 180.f * M_PI);
}
void trig::moveangle(CUserCmd* cmd, const float& y)
{
	trig::moveangle(cmd->forwardmove, cmd->sidemove, y, cmd->viewangles.y);
}

float trig::diffangle(float a1, float a2)
{
	float diff = a1 - a2;
    //if ang > 180 then -360 else if ang < -180 then add 360 else add 0
    return (diff > 180.0f) ? diff - 360.f : (diff < -180.0f) ? diff + 360.f : diff;
}

void trig::aim(CUserCmd* cmd, const Vector& eyes, const Vector& at, bool move)
{
	Vector dist;
	VectorSubtract(at, eyes, dist);

	float y = atan2f(dist.y, dist.x) * 180.f / M_PI;
	float x = atan2f(dist.z, hypotf(dist.x, dist.y)) * -180.f / M_PI;

	if (move)
		trig::moveangle(cmd, y);

	cmd->viewangles.x = x;
	cmd->viewangles.y = y;
}

void trig::aim(QAngle& ang, const Vector& from, const Vector& to)
{
	Vector dist = from - to;

	VectorAngles(dist, ang);
}

} /* namespace skim */
