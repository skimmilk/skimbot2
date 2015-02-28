/*
 * tfdebug.h
 *
 *  Created on: Feb 27, 2015
 *      Author: skim
 */

#ifndef TFDEBUG_H_
#define TFDEBUG_H_

namespace skim
{

class tfdebug
{
public:
	// Spew information about the local player
	static void localinfo();
	// Spew information about the player by client index
	static void playerinfo(int);
};

} /* namespace skim */

#endif /* TFDEBUG_H_ */
