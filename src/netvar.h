/*
 * netvar.h
 *
 *  Created on: Feb 23, 2015
 *      Author: skim
 */

#ifndef NETVAR_H_
#define NETVAR_H_

#include <vector>

#include "sdk/recv.h"
#include "debug.h"

namespace skim
{

class netvar
{
public:
	// Find the offset of a named variable in a class
	static int netoffset(const char* classname,
			const std::vector<const char*>& vars, RecvProp** prop = nullptr);
	static int netoffset(const char* classname, const char* varname,
			RecvProp** prop = nullptr);

	// Weak hashing system, patent holder me don't use or ill sue
#define _id (__LINE__ * 12345 + __COUNTER__ * 54321)

	// Only searches for offset once and stores the value for later
	// Cannot use const char* in template, use short identifier instead
	template <int put__id_here>
	static int off(const char* cname, const char* varname)
	{
		static int offset = 0;
		if (offset) return offset;
		offset = netoffset(cname, varname);
		return offset;
	}
};

} /* namespace skim */

#endif /* NETVAR_H_ */
