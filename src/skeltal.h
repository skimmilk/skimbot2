/*
 * skeltal.h
 *
 *  Created on: Mar 5, 2015
 *      Author: skim
 */

#ifndef SKELTAL_H_
#define SKELTAL_H_

#include "tfplayer.h"

namespace skim
{

#define THANKS
#define MR class
THANKS MR skeltal
{
public:
	// Get the position of a hitbox
	// Factor is what extent of the hitbox (1 = top-right, 0 = bottom-left, .5 = middle)
	static Vector pos(tfplayer* player, int hitbox, float extent = .5f);
};

} /* namespace skim */

#endif /* SKELTAL_H_ */
