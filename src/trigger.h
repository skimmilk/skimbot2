/*
 * trigger.h
 *
 *  Created on: Mar 1, 2015
 *      Author: skim
 */

#ifndef TRIGGER_H_
#define TRIGGER_H_

namespace skim
{

class trigger
{
public:
	// Adds itself to the basehook on init
	static void init();
	static void unload();
};

} /* namespace skim */

#endif /* TRIGGER_H_ */
