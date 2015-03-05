/*
 * cvarhook.h
 *
 *  Created on: Feb 22, 2015
 *      Author: skim
 */

#ifndef CVARHOOK_H_
#define CVARHOOK_H_

#include "sdk/cvar.h"
namespace skim
{

class cvar_hook
{
public:
	// Remove the cheat flag from a convar
	static ConVar* unlock(const char* name);
	static void unlock(ConVar*);

	// Unlock and hide a convar from online server anti-cheats
	// Returns the new convar that is spoofing the variable
	// Servers can query only the value of a convar by name
	static ConVar* hide(const char* name);

	static void init();
};

} /* namespace skim */

#endif /* CVARHOOK_H_ */
