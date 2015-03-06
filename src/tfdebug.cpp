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
#include "tfplayer.h"
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
	tfplayer* player = (tfplayer*)ent->GetBaseEntity();

	if (!player->is_player())
	{
		econ(NAME "Entity is not a player");
		return;
	}

	try
	{
		Vector pos = ent->GetAbsOrigin();
		Vector pos2 = player->m_vecOrigin();
		Vector eye = player->m_vecViewOffset();

		con("ent->GetAbsOrigin():\t\t" + std::to_string(pos.x) +
				", " + std::to_string(pos.y) + ", " + std::to_string(pos.z));
		con("m_vecOrigin:\t\t" + std::to_string(pos2.x) +
				", " + std::to_string(pos2.y) + ", " + std::to_string(pos2.z));
		con("m_vecViewOffset:\t\t" + std::to_string(eye.x) +
				", " + std::to_string(eye.y) + ", " + std::to_string(eye.z));

		con("m_iHealth:\t\t" + std::to_string(player->m_iHealth()));

		con("m_iTeamNum:\t\t" + std::to_string(player->m_iTeamNum()));
		con("m_iClass:\t\t" + std::to_string(player->m_iClass()));
		con("m_nDisguiseClass:\t\t" + std::to_string(player->m_nDisguiseClass()));

		std::bitset<8*4> flags (player->m_nPlayerCond());
		std::bitset<8*4> state (player->m_nPlayerState());
		con("m_nPlayerCond:\t\t" + flags.to_string());
		con("m_nPlayerState:\t\t" + state.to_string());
		con("m_lifeState:\t\t" + std::to_string(player->m_lifeState()));

		// Get the weapon
		con("Weapon information");
		int whandle = player->m_hActiveWeapon();
		con("m_hActiveWeapon:\t\t" + std::to_string(whandle) + " : " +
				std::to_string(whandle & 0x0fff));

		tfweapon* weapon = player->weapon();
		con("m_iItemDefinitionIndex:\t\t" + std::to_string(weapon->m_iItemDefinitionIndex()));
		con("m_flNextPrimaryAttack:\t\t" + std::to_string(weapon->m_flNextPrimaryAttack()));
		con("Weapon class name: " + std::string(weapon->GetClientClass()->m_pNetworkName));
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
