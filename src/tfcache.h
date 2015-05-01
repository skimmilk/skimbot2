/*
 * tfcache.h
 *
 *  Created on: Apr 30, 2015
 *      Author: skim
 */

#ifndef TFCACHE_H_
#define TFCACHE_H_

namespace skim
{

// Holds some information about networked entities
class tfcache
{
public:
	static void init();
	static bool is_moving(int entnum);
};

} /* namespace skim */

#endif /* TFCACHE_H_ */
