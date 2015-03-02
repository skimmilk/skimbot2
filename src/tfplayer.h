/*
 * tfplayer.h
 *
 *  Created on: Mar 1, 2015
 *      Author: skim
 */

#ifndef TFPLAYER_H_
#define TFPLAYER_H_

#include "ifs.h"
#include "netvar.h"
#include "sdk/common.h"
#include "sdk/engine.h"
#include "sdk/cliententity.h"

namespace skim
{

enum class tfclass : int { scout = 1, sniper, soldier, demoman, medic, heavy, pyro, spy, engineer };
enum class tfhitbox : int
{
    HITBOX_ENGINEER_HIP_R = 14,
    HITBOX_ENGINEER_KNEE_R = 15,
    HITBOX_ENGINEER_SPINE_0 = 1,
    HITBOX_ENGINEER_SPINE_1 = 2,
    HITBOX_ENGINEER_SPINE_2 = 3,
    HITBOX_ENGINEER_SPINE_3 = 4,
    HITBOX_ENGINEER_HEAD = 0,
    HITBOX_ENGINEER_HIP_L = 11,
    HITBOX_ENGINEER_KNEE_L = 12,
    HITBOX_ENGINEER_UPPERARM_L = 5,
    HITBOX_ENGINEER_LOWERARM_L = 6,
    HITBOX_ENGINEER_UPPERARM_R = 8,
    HITBOX_ENGINEER_LOWERARM_R = 9,
    HITBOX_ENGINEER_FOOT_L = 13,
    HITBOX_ENGINEER_FOOT_R = 16,
    HITBOX_ENGINEER_HAND_R = 10,
    HITBOX_ENGINEER_HAND_L = 7,

    HITBOX_PELVIS = 1,
    HITBOX_SPINE_0 = 2,
    HITBOX_SPINE_1 = 3,
    HITBOX_SPINE_2 = 4,
    HITBOX_SPINE_3 = 5,
    HITBOX_HEAD = 0,
    HITBOX_HIP_R = 15,
    HITBOX_KNEE_R = 16,
    HITBOX_FOOT_R = 17,
    HITBOX_UPPERARM_L = 6,
    HITBOX_LOWERARM_L = 7,
    HITBOX_HAND_L = 8,
    HITBOX_HIP_L = 12,
    HITBOX_KNEE_L = 13,
    HITBOX_FOOT_L = 14,
    HITBOX_SCOUT_PACKMIDDLE = 18,
    HITBOX_UPPERARM_R = 9,
    HITBOX_LOWERARM_R = 10,
    HITBOX_HAND_R = 11,

    HITBOX_ORIGIN = -1
};
enum class bonepos { MAX, MIN, MIDDLE };


#define DEFNETVAR(result, name, ...) result& name() { return netvar::net<_id, result>(this, __VA_ARGS__, #name); }
class tfplayer : public IClientEntity
{
public:
	inline static tfplayer* me() { return (tfplayer*) ifs::entities->GetClientEntity(ifs::engine->GetLocalPlayer()); }

	DEFNETVAR(int, m_iHealth, "CBasePlayer");
	DEFNETVAR(int, m_lifeState, "CBasePlayer");
	DEFNETVAR(int, m_fFlags, "CBasePlayer");
	DEFNETVAR(Vector, m_vecOrigin, "CBasePlayer");
	DEFNETVAR(int, m_iTeamNum, "CBasePlayer");
	DEFNETVAR(int, m_hActiveWeapon, "CBasePlayer");
	DEFNETVAR(int, m_bGlowEnabled, "CBasePlayer");

	DEFNETVAR(int, m_iClass, "CTFPlayer", "m_PlayerClass");
	DEFNETVAR(int, m_nPlayerCond, "CTFPlayer", "m_Shared");
	DEFNETVAR(bool, m_bJumping, "CTFPlayer", "m_Shared");
	DEFNETVAR(int, m_nPlayerState, "CTFPlayer", "m_Shared");
	DEFNETVAR(float, m_flChargeMeter, "CTFPlayer", "m_Shared");
	DEFNETVAR(int, m_nDisguiseClass, "CTFPlayer", "m_Shared");
	DEFNETVAR(bool, m_bFeignDeathReady, "CTFPlayer", "m_Shared");
	DEFNETVAR(int, m_iDecapitations, "CTFPlayer", "m_Shared");
	DEFNETVAR(int, m_iRevengeCrits, "CTFPlayer", "m_Shared");

	// LOCAL ONLY CBasePlayer.localdata.m_vecViewOffset[0]
	Vector& m_vecViewOffset()
	{
		return netvar::net<_id, Vector>(this, "CBasePlayer", "localdata", "m_vecViewOffset[0]");
	}
	Vector local_eyes()
	{
		Vector ret;
		Vector pos = m_vecOrigin();
		Vector off = m_vecViewOffset();
		VectorAdd(pos, off, ret);
		return ret;
	}
};

class tfweapon
{
public:
	DEFNETVAR(float, m_flNextPrimaryAttack, "CBaseCombatWeapon", "LocalActiveWeaponData", "m_flNextPrimaryAttack");
	DEFNETVAR(int, m_iState, "CBaseCombatWeapon");
	DEFNETVAR(int, m_nKillComboCount, "CTFWeaponBase");
};

class tfobject
{
public:
	DEFNETVAR(int, m_iHealth, "CBaseObject");
	DEFNETVAR(int, m_iMaxHealth, "CBaseObject");
	DEFNETVAR(int, m_fObjectFlags, "CBaseObject");
};

class tfprojectile
{
public:
	DEFNETVAR(Vector, m_vecOrigin, "CBaseProjectile");
	DEFNETVAR(int, m_iTeamNum, "CBaseProjectile");
	DEFNETVAR(int, m_hOwnerEntity, "CBaseProjectile");
	DEFNETVAR(int, m_hOriginalLauncher, "CBaseProjectile");

	DEFNETVAR(int, m_hLauncher, "CTFBaseProjectile");
};

}

#endif /* TFPLAYER_H_ */
