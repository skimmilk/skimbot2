/*
 * steam.h
 *
 *  Created on: Apr 18, 2015
 *      Author: skim
 */

#ifndef STEAMUTIL_H_
#define STEAMUTIL_H_

namespace skim
{

class steam
{
public:
	// Determines if player is a steam friend by entity number
	static bool is_friend(int entnum);
};

} /* namespace skim */

#endif /* STEAMUTIL_H_ */
