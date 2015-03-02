/*
 * bhop.h
 *
 *  Created on: Mar 2, 2015
 *      Author: skim
 */

#ifndef BHOP_H_
#define BHOP_H_

namespace skim
{

class bhop
{
public:
	// Does the obvious and adds itself to the basehook
	static void init();
	static void unload();
};

} /* namespace skim */

#endif /* BHOP_H_ */
