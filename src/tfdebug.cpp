/*
 * tfdebug.cpp
 *
 *  Created on: Feb 27, 2015
 *      Author: skim
 */

#include "tfdebug.h"

#include <bitset>
#include <string>
#include "ifs.h"
#include "sourceutil.h"
#include "netvar.h"
#include "const.h"
#include "sdk/cliententity.h"
#include "sdk/engine.h"

namespace skim
{

void tfdebug::playerinfo(int index)
{
	if (!index)
	{
		econ(NAME "No player selected");
		return;
	}

	IClientEntity* ent = ifs::entities->GetClientEntity(index);
	if (!index)
	{
		econ(NAME "No entity found at specified index");
		return;
	}

	try
	{
		C_BaseEntity* player = ent->GetBaseEntity();
		Vector pos = ent->GetAbsOrigin();
		Vector pos2 = netvar::net<_id, Vector>(player, "CBasePlayer", "m_vecOrigin");

		con("ent->GetAbsOrigin():\t\t" + std::to_string(pos.x) +
				", " + std::to_string(pos.y) + ", " + std::to_string(pos.z));
		con("m_vecOrigin:\t\t" + std::to_string(pos2.x) +
				", " + std::to_string(pos2.y) + ", " + std::to_string(pos2.z));

		con("m_iHealth:\t\t" + std::to_string(
				netvar::net<_id, int>(player, "CBasePlayer", "m_iHealth")));

		con("m_iTeamNum:\t\t" + std::to_string(
				netvar::net<_id, int>(player, "CBasePlayer", "m_iTeamNum")));
		con("m_iClass:\t\t" + std::to_string(
				netvar::net<_id, int>(player, "CTFPlayer", "m_PlayerClass", "m_iClass")));
		con("m_nDisguiseClass:\t\t" + std::to_string(
				netvar::net<_id, int>(player, "CTFPlayer", "m_Shared", "m_nDisguiseClass")));

		std::bitset<8*4> flags (netvar::net<_id, int>(player, "CTFPlayer", "m_Shared", "m_nPlayerCond"));
		con("m_nPlayerCond:\t\t" + flags.to_string());
	}
	catch (...)
	{
		econ(PREFIX_ERROR "Crashed and burned while grabbing information");
	}
}

void tfdebug::localinfo()
{
	playerinfo(ifs::engine->GetLocalPlayer());
}

} /* namespace skim */
