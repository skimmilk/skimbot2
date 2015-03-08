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
#include "sdk/vars.h"

namespace skim
{

void tfdebug::playerinfo(int index)
{
	if (!index)
	{
		econ(NAME "No player selected");
		return;
	}

	tfplayer* player = (tfplayer*)ifs::entities->GetClientEntity(index);
	if (!player)
	{
		econ(NAME "No entity found at specified index");
		return;
	}
	if (!player->is_player())
	{
		econ(NAME "Entity is not a player");
		return;
	}

	Vector pos = player->GetAbsOrigin();
	Vector pos2 = player->m_vecOrigin();
	Vector eye = player->m_vecViewOffset();
	QAngle look = player->GetAbsAngles();

	con("GetAbsOrigin():\t\t" + std::to_string(pos.x) +
			", " + std::to_string(pos.y) + ", " + std::to_string(pos.z));
	con("GetAbsAngles():\t\t" + std::to_string(look.x) +
			", " + std::to_string(look.y) + ", " + std::to_string(look.z));
	con("m_vecOrigin:\t\t" + std::to_string(pos2.x) +
			", " + std::to_string(pos2.y) + ", " + std::to_string(pos2.z));
	con("m_vecViewOffset:\t\t" + std::to_string(eye.x) +
			", " + std::to_string(eye.y) + ", " + std::to_string(eye.z));

	con("m_iHealth:\t\t" + std::to_string(player->m_iHealth()));

	con("m_iTeamNum:\t\t" + std::to_string(player->m_iTeamNum()));
	con("m_iClass:\t\t" + std::to_string(player->m_iClass()));
	con("m_nDisguiseClass:\t\t" + std::to_string(player->m_nDisguiseClass()));

	con("m_nPlayerCond:\t\t" + std::to_string(player->m_nPlayerCond()));
	con("m_nPlayerState:\t\t" + std::to_string(player->m_nPlayerState()));
	con("m_lifeState:\t\t" + std::to_string(player->m_lifeState()));

	int whandle = player->m_hActiveWeapon();
	if (!whandle || whandle == -1)
		return;
	// Get the weapon
	con("Weapon information");
	con("m_hActiveWeapon:\t\t" + std::to_string(whandle) + " : " +
			std::to_string(whandle & 0x0fff));

	tfweapon* weapon = player->weapon();
	if (!weapon)
		return;
	con("m_iItemDefinitionIndex:\t\t" + std::to_string(weapon->m_iItemDefinitionIndex()));
	con("m_flNextPrimaryAttack:\t\t" + std::to_string(weapon->m_flNextPrimaryAttack()));
	con("m_iReloadMode:\t\t" + std::to_string(weapon->m_iReloadMode()));
	con("reloading:\t\t" + std::to_string(weapon->m_bInReload()));
	con("Weapon class name: " + std::string(weapon->GetClientClass()->m_pNetworkName));
	con("Slot/streaming/damaging:\t\t" + std::to_string((int)weapon->slot()) +
			std::to_string(weapon->streaming()) +
			std::to_string(weapon->damaging()));
}

void tfdebug::localinfo()
{
	playerinfo(ifs::engine->GetLocalPlayer());
}

} /* namespace skim */
