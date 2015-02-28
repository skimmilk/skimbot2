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

	// Dump offsets to console
	static void dumpnets();
	// Dump the names of all classes
	static void dumpclasses();
	// Dump all the netvars for a class by name
	static void dumpnets(const char* classname);

	// Weak hashing system, patent holder me don't use or ill sue
#define _id (__LINE__ * 12345 + __COUNTER__ * 54321)

	// Only searches for offset once and stores the value for later
	// Cannot use const char* in template, use short identifier instead
	template <int put__id_here, typename... Args>
	static int off(const char* cname, Args... args)
	{
		static int offset = 0;
		if (offset) return offset;
		offset = netoffset(cname, std::vector<const char*>{args...});
		return offset;
	}

	// Get the value using the offset of the netvars
	template <int put__id_here, typename Type, typename Class, typename... Args>
	static Type& net(Class* obj, Args... args)
	{
		int offset = off<put__id_here>(args...);
		return *(Type*)((char*)obj + offset);
	}
};

} /* namespace skim */

#endif /* NETVAR_H_ */
