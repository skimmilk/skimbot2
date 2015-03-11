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
#include "sdk/cliententity.h"

namespace skim
{

enum class tftype { other, player, object, projectile };
enum class tftype_object : int { other, sentry, dispenser, teleporter, health, ammo, money };
enum class tftype_projectile : int { other, sticky, pill, rocket, jar, ball, cleaver, flare, arrow };
enum class tfclass : int { scout = 1, sniper, soldier, demoman, medic, heavy, pyro, spy, engineer };
// tfslot::menu describes the engineer's build/destroy and the spy's disguise menus
enum class tfslot : int { primary = 1, secondary, melee, menu };

// Flags in m_nPlayerState
#define STATE_DYING 1
#define STATE_DEAD 2

// Condition flags for m_nPlayerCond
#define COND_DISGUISED 8
#define COND_CLOAKED 16
#define COND_TAUNT 128

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

#define DEFNETVAR(result, name, ...) result& name() { return netvar::net<_id, result>(this, __VA_ARGS__, #name); }

class tfentity : public IClientEntity
{
public:
	tftype type();
};

class tfweapon;
class tfplayer : public tfentity
{
public:
	static tfplayer* me();

	DEFNETVAR(int, m_iHealth, "CBasePlayer");
	DEFNETVAR(int, m_fFlags, "CBasePlayer");
	DEFNETVAR(Vector, m_vecOrigin, "CBasePlayer");
	DEFNETVAR(int, m_iTeamNum, "CBasePlayer");
	DEFNETVAR(int, m_hActiveWeapon, "CBasePlayer");
	DEFNETVAR(int, m_bGlowEnabled, "CBasePlayer");

	DEFNETVAR(tfclass, m_iClass, "CTFPlayer", "m_PlayerClass");
	DEFNETVAR(int, m_nPlayerCond, "CTFPlayer", "m_Shared");
	DEFNETVAR(bool, m_bJumping, "CTFPlayer", "m_Shared");
	DEFNETVAR(int, m_nPlayerState, "CTFPlayer", "m_Shared");
	DEFNETVAR(float, m_flChargeMeter, "CTFPlayer", "m_Shared");
	DEFNETVAR(int, m_nDisguiseClass, "CTFPlayer", "m_Shared");
	DEFNETVAR(bool, m_bFeignDeathReady, "CTFPlayer", "m_Shared");
	DEFNETVAR(int, m_iDecapitations, "CTFPlayer", "m_Shared");
	DEFNETVAR(int, m_iRevengeCrits, "CTFPlayer", "m_Shared");

	bool is_player();
	bool is_alive() { return !(m_nPlayerState() & STATE_DEAD || m_nPlayerState() & STATE_DYING); }
	bool is_drawable() { return is_alive() && !this->IsDormant(); }

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
	// Get the weapon of this player
	tfweapon* weapon();
};

class tfweapon : public tfentity
{
public:
	DEFNETVAR(int, m_iClip1, "CBaseCombatWeapon", "LocalWeaponData");
	DEFNETVAR(int, m_iClip2, "CBaseCombatWeapon", "LocalWeaponData");
	DEFNETVAR(float, m_flNextPrimaryAttack, "CBaseCombatWeapon", "LocalActiveWeaponData");
	DEFNETVAR(float, m_flNextSecondaryAttack, "CBaseCombatWeapon", "LocalActiveWeaponData");
	DEFNETVAR(int, m_iState, "CBaseCombatWeapon");
	DEFNETVAR(int, m_iItemDefinitionIndex, "CBaseCombatWeapon", "m_AttributeManager", "m_Item");
	DEFNETVAR(int, m_nKillComboCount, "CTFWeaponBase");
	DEFNETVAR(bool, m_bReadyToBackstab, "CTFKnife");
	// m_iReloadMode 1 means mandatory reload (usually), 2 = can fire while reloading (usually)
	// Deals with primary weapons (usually)
	DEFNETVAR(int, m_iReloadMode, "CTFWeaponBase");
	// Deals with secondary weapons (usually)
	char& m_bInReload()
	{
		int offset = netvar::off<_id>("CBaseCombatWeapon", "LocalActiveWeaponData", "m_flNextPrimaryAttack") + 12;
		char* ptr = (char*)this;
		ptr += offset;
		return *ptr;
	}

	tfslot slot();
	// Returns true if this weapon is a streaming weapon (flamethrower, pistol, minigun, etc.)
	bool streaming();
	// Can this weapon can do damage
	bool damaging();
};

class tfobject : public tfentity
{
public:
	DEFNETVAR(int, m_iTeamNum, "CBaseObject");
	DEFNETVAR(int, m_iHealth, "CBaseObject");
	DEFNETVAR(int, m_iMaxHealth, "CBaseObject");
	DEFNETVAR(int, m_fObjectFlags, "CBaseObject");

	tftype_object object_type();
};

class tfprojectile : public tfentity
{
public:
	DEFNETVAR(Vector, m_vecOrigin, "CBaseProjectile");
	DEFNETVAR(int, m_iTeamNum, "CBaseProjectile");
	DEFNETVAR(int, m_hOwnerEntity, "CBaseProjectile");
	DEFNETVAR(int, m_hOriginalLauncher, "CBaseProjectile");
	DEFNETVAR(int, m_hLauncher, "CTFBaseProjectile");

	tftype_projectile projectile_type();
};

}

#endif /* TFPLAYER_H_ */
