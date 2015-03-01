/*
 * tfplayer.h
 *
 *  Created on: Mar 1, 2015
 *      Author: skim
 */

#ifndef TFPLAYER_H_
#define TFPLAYER_H_

#include "netvar.h"
#include "sdk/common.h"

namespace skim
{

#define DEFNETVAR(result, name, ...) result& name() { return netvar::net<_id, result>(this, __VA_ARGS__, #name); }
class tfplayer
{
public:
	DEFNETVAR(int, m_iHealth, "CBasePlayer");
	DEFNETVAR(int, m_fFlags, "CBasePlayer");
	DEFNETVAR(Vector, m_vecOrigin, "CBasePlayer");
	DEFNETVAR(int, m_iTeamNum, "CBasePlayer");
	DEFNETVAR(int, m_hActiveWeapon, "CBasePlayer");

	DEFNETVAR(int, m_iClass, "CTFPlayer", "m_PlayerClass");
	DEFNETVAR(int, m_nPlayerCond, "CTFPlayer", "m_Shared");
	DEFNETVAR(bool, m_bJumping, "CTFPlayer", "m_Shared");
	DEFNETVAR(int, m_nPlayerState, "CTFPlayer", "m_Shared");
	DEFNETVAR(float, m_flChargeMeter, "CTFPlayer", "m_Shared");
	DEFNETVAR(int, m_nDisguiseClass, "CTFPlayer", "m_Shared");
};

class tfweapon
{
public:
	DEFNETVAR(float, m_flNextPrimaryAttack, "CBaseCombatWeapon", "LocalActiveWeaponData", "m_flNextPrimaryAttack");
	DEFNETVAR(int, m_iState, "CBaseCombatWeapon");
};

class tfobject
{
public:
	DEFNETVAR(int, m_iHealth, "CBaseObject");
	DEFNETVAR(int, m_iMaxHealth, "CBaseObject");
	DEFNETVAR(int, m_fObjectFlags, "CBaseObject");
};

}

#endif /* TFPLAYER_H_ */
