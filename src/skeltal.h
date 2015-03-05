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

// What part of the bone do we want
enum class extents { min, max, middle };

#define THANKS
#define MR class
THANKS MR skeltal
{
public:
	// Get the position of a hitbox
	// Factor is what extent of the hitbox (1 = top-right, 0 = bottom-left, .5 = middle)
	static Vector pos(tfplayer* player, int hitbox, extents ext = extents::middle);

	// Get a simple bounding box for a hitbox
	static void bbox(tfplayer* player, int hitbox, Vector& min, Vector& max);
};

} /* namespace skim */

#endif /* SKELTAL_H_ */
