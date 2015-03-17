/*
 * esp.h
 *
 *  Created on: Mar 5, 2015
 *      Author: skim
 */

#ifndef ESP_H_
#define ESP_H_

#include "sdk/common.h"

namespace skim
{

class esp
{
public:
	// Initializes ESP system and hooks PaintTraverse and CreateMove
	// Should come first because this cleans the highlight buffer
	static void init();

	// Highlight the entity with this color
	// Only call in CreateMove hooks
	static void highlight(int index, const color& c);
	// Highlight with a light color
	static void highlight(int index);
};

} /* namespace skim */

#endif /* ESP_H_ */
