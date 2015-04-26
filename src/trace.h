/*
 * trace.h
 *
 *  Created on: Mar 1, 2015
 *      Author: skim
 *
 * Purpose: Assists in 3D line-of-sight spatial programming
 */

#ifndef TRACE_H_
#define TRACE_H_

#include "sdk/common.h"

class IClientEntity;

namespace skim
{
class tfentity;

class trace
{
public:
	// Returns what the ray hits (ray has MASK_SHOT)
	static IClientEntity* ray(const Vector& start, const QAngle& aim, int ignore);
	// Returns the the player in the line of sight
	static tfentity* sight(const Vector& start, const QAngle& viewangle, int idignore);
	// Determines if player can see player, quick
	static bool can_see_fast(const Vector& start, tfentity* pl);
	static bool can_see_fast(const Vector& start, const Vector& end, tfentity* filter);
};

} /* namespace skim */

#endif /* TRACE_H_ */
