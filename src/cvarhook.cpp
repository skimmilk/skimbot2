/*
 * cvarhook.cpp
 *
 *  Created on: Feb 22, 2015
 *      Author: skim
 */

#include "cvarhook.h"

#include <vector>
#include <string>
#include <tuple>
#include <string.h>

#include "ifs.h"
#include "sourceutil.h"
#include "const.h"
#include "exit.h"

namespace skim
{

// ConVars that were created to bypass detections
static std::vector<ConVar*> created;

// Holds all convars that were changed
// the original convar, name, flags, min and max
static std::vector<
		std::tuple<ConVar*, const char*, int, bool, bool>
	> hooked;

void cvar_hook::unlock(ConVar* original)
{
	// Backup the original's information
	hooked.push_back(std::make_tuple(original,
			original->m_pszString, original->m_nFlags,
			original->m_bHasMin, original->m_bHasMax));

	original->m_bHasMax = original->m_bHasMin = false;
	original->m_nFlags &= ~FCVAR_CHEAT;
}
ConVar* cvar_hook::unlock(const char* name)
{
	ConVar* result = ifs::cvar->FindVar(name);
	if (!result)
		return nullptr;

	unlock(result);
	return result;
}

ConVar* cvar_hook::hide(const char* name)
{
	// Change the name of the cvar referenced by name to something else
	// Make new cvar with the same name that gets exposed to server anti-cheats
	ConVar* original = ifs::cvar->FindVar(name);
	if (!original) return 0;

	// Create the new name for it
	std::string newname = PREFIX "hooked_" + std::string(name);
	std::string originalname (original->m_pszName);

	// Variable to take the original's place
	ConVar* newvar = new ConVar(originalname.c_str(), original->m_pszDefaultValue,
			original->m_nFlags, "Hooked convar");
	newvar->m_fMaxVal = original->m_fMaxVal;
	newvar->m_fMinVal = original->m_fMinVal;
	newvar->m_bHasMax = original->m_bHasMax;
	newvar->m_bHasMin = original->m_bHasMin;

	// Replace the name
	original->m_pszName = (const char*)malloc(newname.size() + 1);
	strcpy((char*)original->m_pszName, newname.c_str());

	// Unlock the original
	unlock(original);

	created.push_back(newvar);
	return newvar;
}

static ConCommand* hidecommand;

static void cmd_hide(const CCommand& args)
{
	if (!strlen(args.Arg(1))){
		con("Usage: " PREFIX "cvar_hide <cvar>");
		return;
	}
	if (cvar_hook::hide(args.Arg(1)) == nullptr)
		con("Could not find cvar");
}

static void unload()
{
	delete hidecommand;
	// TODO: Restore commands
}

void cvar_hook::init()
{
	hidecommand = new ConCommand(PREFIX "cvar_hide", cmd_hide,
			"Hide a cvar from anticheats and unlock it");
	exit::handle(unload);
}

}; /* namespace skim */
