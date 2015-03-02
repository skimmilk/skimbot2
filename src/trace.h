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

#include "tfplayer.h"
#include "sdk/common.h"

namespace skim
{

class trace
{
	// Returns the the player in the line of sight
	static tfplayer* sight(const Vector& start, QAngle viewangle, int idignore);
};

} /* namespace skim */

#endif /* TRACE_H_ */
