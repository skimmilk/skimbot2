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
#include "sdk/model.h"

namespace skim
{

void tfdebug::playerinfo(int index)
{
	if (index < 1)
	{
		econ(NAME "No entity selected");
		return;
	}

	tfplayer* player = (tfplayer*)ifs::entities->GetClientEntity(index);
	if (!player)
	{
		econ(NAME "No entity found at specified index");
		return;
	}

	con(player->GetClientClass()->m_pNetworkName);

	const model_t* model = player->GetModel();
	if (!model)
		return;
	studiohdr_t* studio = ifs::model_info->GetStudiomodel(model);
	if (!studio)
		return;

	con(std::string(studio->name, studio->length));
	con(std::to_string(studio->checksum));

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

	if (player->type() == tftype::player)
	{
		con("m_iHealth:\t\t" + std::to_string(player->m_iHealth()));

		con("m_iTeamNum:\t\t" + std::to_string(player->m_iTeamNum()));
		con("m_iClass:\t\t" + std::to_string((int)player->m_iClass()));
		con("m_nDisguiseClass:\t\t" + std::to_string(player->m_nDisguiseClass()));

		con("m_nPlayerCond:\t\t" + std::to_string(player->m_nPlayerCond()));
		con("m_nPlayerState:\t\t" + std::to_string(player->m_nPlayerState()));

		if (player->type() != tftype::player)
			return;

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
	else if (player->type() == tftype::object)
	{
		tfobject* obj = (tfobject*)player;

		con("m_iHealth:\t\t" + std::to_string(obj->m_iHealth()));
		con("m_iMaxHealth:\t\t" + std::to_string(obj->m_iMaxHealth()));
		con("m_fObjectFlags:\t\t" + std::to_string(obj->m_fObjectFlags()));
		con("type:\t\t" + std::to_string((int)obj->object_type()));
	}
	else if (player->type() == tftype::projectile)
	{
		tfprojectile* proj = (tfprojectile*)player;

		con("m_iTeamNum:\t\t" + std::to_string(proj->m_iTeamNum()));
		con("type:\t\t" + std::to_string((int)proj->projectile_type()));
	}
}

void tfdebug::localinfo()
{
	playerinfo(ifs::engine->GetLocalPlayer());
}

} /* namespace skim */
