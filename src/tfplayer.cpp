/*
 * tfplayer.cpp
 *
 *  Created on: Mar 6, 2015
 *      Author: skim
 */

#include "tfplayer.h"

#include <unordered_map>
#include <unordered_set>
#include "ifs.h"
#include "const.h"
#include "sourceutil.h"
#include "sdk/engine.h"
#include "sdk/vars.h"
#include "sdk/model.h"

namespace skim
{

tfplayer* tfplayer::me()
{
	return (tfplayer*) ifs::entities->GetClientEntity(ifs::engine->GetLocalPlayer());
}
tfweapon* tfplayer::weapon()
{
	return (tfweapon*)ifs::entities->GetClientEntity(m_hActiveWeapon() & 0x0fff);
}
bool tfplayer::is_player()
{
	return entindex() > 0 && entindex() <= ifs::engine->GetMaxClients();
}

// Register weapon and slot
#define REGWEP(str, slot) weapon_slot[netvar::class_id_search(#str)] = (tfslot)slot
// Register weapon information that it streams (for critical hits)
#define REGWEPSTREAM(str) weapon_streaming.insert(netvar::class_id_search(#str))
// Weapon does not do any damage
#define REGWEPNODMG(str) weapon_nodmg.insert(netvar::class_id_search(#str))
// Register client class type
#define REGENT(str, type) entity_type[netvar::class_id_search(#str)] = std::make_tuple(type, 0)
#define REGENT2(str, type, second) entity_type[netvar::class_id_search(#str)] = std::make_tuple(type, (int)second)

// First parameter is class ID, second slot
std::unordered_map<int, tfslot> weapon_slot;
std::unordered_set<int> weapon_streaming;
std::unordered_set<int> weapon_nodmg;
std::unordered_map<int, std::tuple<tftype, int>> entity_type;
bool initialized;

static void init()
{
	initialized = true;
	REGENT(CTFPlayer, tftype::player);
	REGENT2(CObjectSentrygun, tftype::object, tftype_object::sentry);
	REGENT2(CObjectDispenser, tftype::object, tftype_object::dispenser);
	REGENT2(CObjectTeleporter, tftype::object, tftype_object::teleporter);
	REGENT2(CTFAmmoPack, tftype::object, tftype_object::ammo);
	REGENT2(CTFProjectile_Rocket, tftype::projectile, tftype_projectile::rocket);
	REGENT2(CTFProjectile_Flare, tftype::projectile, tftype_projectile::flare);
	REGENT2(CTFProjectile_HealingBolt, tftype::projectile, tftype_projectile::arrow);
	REGENT2(CTFProjectile_Arrow, tftype::projectile, tftype_projectile::arrow);
	REGENT2(CTFProjectile_SentryRocket, tftype::projectile, tftype_projectile::rocket);
	REGENT2(CTFProjectile_Throwable, tftype::projectile, tftype_projectile::cleaver);
	REGENT2(CTFStunBall, tftype::projectile, tftype_projectile::ball);
	REGENT2(CTFBall_Ornament, tftype::projectile, tftype_projectile::ball);
	REGENT(CTFGrenadePipebombProjectile, tftype::projectile); // sticky&pill

	REGWEP(CTFWearableRobotArm, 3);
	REGWEP(CTFRobotArm, 3);
	REGWEP(CTFWrench, 3);
	REGWEP(CTFSyringeGun, 1);
	REGWEPSTREAM(CTFSyringeGun);
	REGWEP(CTFKatana, 3);
	REGWEP(CTFSword, 3);
	REGWEP(CTFSniperRifleClassic, 1);
	REGWEP(CTFSniperRifleDecap, 1);
	REGWEP(CTFSniperRifle, 1);
	REGWEP(CTFSMG, 2);
	REGWEPSTREAM(CTFSMG);
	REGWEP(CTFShovel, 3);
	REGWEP(CTFShotgunBuildingRescue, 1);
	REGWEP(CTFPEPBrawlerBlaster, 1);
	REGWEP(CTFSodaPopper, 1);
	REGWEP(CTFShotgun_Revenge, 1);
	REGWEP(CTFScatterGun, 1);
	REGWEP(CTFShotgun_Pyro, 2);
	REGWEP(CTFShotgun_HWG, 2);
	REGWEP(CTFShotgun_Soldier, 2);
	REGWEP(CTFShotgun, 2);
	REGWEP(CTFCrossbow, 1);
	REGWEP(CTFRocketLauncher_Mortar, 1);
	REGWEP(CTFRocketLauncher_AirStrike, 1);
	REGWEP(CTFRocketLauncher_DirectHit, 1);
	REGWEP(CTFRocketLauncher, 1);
	REGWEP(CTFRevolver, 1);
	REGWEP(CTFDRGPomson, 1);
	REGWEP(CTFRaygun, 2);
	REGWEP(CTFPistol_ScoutSecondary, 2);
	REGWEPSTREAM(CTFPistol_ScoutSecondary);
	REGWEP(CTFPistol_ScoutPrimary, 1); // shortstop
	REGWEPSTREAM(CTFPistol_ScoutPrimary);
	REGWEP(CTFPistol_Scout, 2);
	REGWEPSTREAM(CTFPistol_Scout);
	REGWEP(CTFPistol, 2);
	REGWEPSTREAM(CTFPistol);
	REGWEP(CTFPipebombLauncher, 2);
	REGWEP(CTFWeaponPDA_Spy, 4);
	REGWEPNODMG(CTFWeaponPDA_Spy);
	REGWEP(CTFWeaponPDA_Engineer_Destroy, 4);
	REGWEPNODMG(CTFWeaponPDA_Engineer_Destroy);
	REGWEP(CTFWeaponPDA_Engineer_Build, 4);
	REGWEPNODMG(CTFWeaponPDA_Engineer_Build);
	REGWEP(CTFWeaponPDAExpansion_Teleporter, 4);
	REGWEPNODMG(CTFWeaponPDAExpansion_Teleporter);
	REGWEP(CTFWeaponPDAExpansion_Dispenser, 4);
	REGWEPNODMG(CTFWeaponPDAExpansion_Dispenser);
	REGWEP(CTFWeaponPDA, 4);
	REGWEPNODMG(CTFWeaponPDA);
	REGWEP(CTFParticleCannon, 1);
	REGWEP(CTFMinigun, 1);
	REGWEPSTREAM(CTFMinigun);
	REGWEP(CTFMedigunShield, 2);
	REGWEPNODMG(CTFMedigunShield);
	REGWEP(CWeaponMedigun, 2);
	REGWEPNODMG(CWeaponMedigun);
	REGWEP(CTFMechanicalArm, 2);
	REGWEP(CTFLunchBox_Drink, 2);
	REGWEP(CTFLunchBox, 2);
	REGWEP(CTFKnife, 3);
	REGWEP(CTFCleaver, 3);
	REGWEP(CTFJarMilk, 2);
	REGWEP(CTFJar, 2);
	REGWEP(CTFWeaponInvis, 4);
	REGWEPNODMG(CTFWeaponInvis);
	REGWEP(CTFCannon, 1);
	REGWEP(CTFGrenadeLauncher, 1);
	REGWEP(CTFFlareGun_Revenge, 2);
	REGWEP(CTFFlareGun, 2);
	REGWEP(CTFFlameRocket, 1);
	REGWEP(CTFFlameThrower, 1);
	REGWEPSTREAM(CTFFlameThrower);
	REGWEP(CTFFists, 3);
	REGWEP(CTFFireAxe, 3);
	REGWEP(CTFCompoundBow, 1);
	REGWEP(CTFClub, 3);
	REGWEP(CTFBuffItem, 2);
	REGWEPNODMG(CTFBuffItem);
	REGWEP(CTFStickBomb, 2);
	REGWEP(CTFBottle, 3);
	REGWEP(CTFBonesaw, 3);
	REGWEP(CTFBat_Giftwrap, 3);
	REGWEP(CTFBat_Wood, 3);
	REGWEP(CTFBat_Fish, 3);
	REGWEP(CTFBat, 3);
	REGWEP(CTFWeaponBuilder, 4);
	REGWEPNODMG(CTFWeaponBuilder);
}
tfslot tfweapon::slot()
{
	if (!initialized)
		init();

	auto needle = weapon_slot.find(this->GetClientClass()->m_ClassID);
	if (needle == weapon_slot.end())
	{
		econ(NAME "Could not find the weapon slot for weapon " +
				std::string(this->GetClientClass()->m_pNetworkName));
		return tfslot::primary;
	}
	return needle->second;
}
bool tfweapon::streaming()
{
	if (!initialized)
		init();

	return weapon_streaming.count(this->GetClientClass()->m_ClassID);
}
bool tfweapon::damaging()
{
	if (!initialized)
		init();

	return !weapon_nodmg.count(this->GetClientClass()->m_ClassID);
}

tftype tfentity::type()
{
	if (!initialized)
		init();

	auto needle = entity_type.find(GetClientClass()->m_ClassID);
	if (needle == entity_type.end())
		return tftype::other;
	return std::get<0>(needle->second);
}
tftype_object tfobject::object_type()
{
	if (!initialized)
		init();

	auto needle = entity_type.find(GetClientClass()->m_ClassID);
	if (needle == entity_type.end())
		return tftype_object::other;
	return (tftype_object)std::get<1>(needle->second);

}
tftype_projectile tfprojectile::projectile_type()
{
	if (!initialized)
		init();

	int classid = GetClientClass()->m_ClassID;
	if (classid == netvar::classid<_id>("CTFGrenadePipebombProjectile"))
	{
		// Get model of entity
		studiohdr_t* studio = ifs::model_info->GetStudiomodel(GetModel());
		if (studio->checksum == 959466927 ||
				studio->checksum == -374341724)
			return tftype_projectile::sticky;
		return tftype_projectile::pill;
	}

	auto needle = entity_type.find(classid);
	if (needle == entity_type.end())
		return tftype_projectile::other;
	return (tftype_projectile)std::get<1>(needle->second);

}
const char* tfentity::type_name()
{
	auto t = type();
	if (t == tftype::player)
		switch (((tfplayer*)this)->m_iClass())
		{
		case tfclass::demoman:
			return "Demoman";
		case tfclass::engineer:
			return "Engineer";
		case tfclass::heavy:
			return "Heavy";
		case tfclass::medic:
			return "Medic";
		case tfclass::pyro:
			return "Pyro";
		case tfclass::scout:
			return "Scout";
		case tfclass::sniper:
			return "Sniper";
		case tfclass::soldier:
			return "Soldier";
		case tfclass::spy:
			return "Spy";
		default:
			return "Unknown";
		}
	else if (t == tftype::object)
		switch (((tfobject*)this)->object_type())
		{
		case tftype_object::ammo:
			return "Ammo";
		case tftype_object::dispenser:
			return "Dispenser";
		case tftype_object::health:
			return "Health";
		case tftype_object::money:
			return "Money";
		case tftype_object::sentry:
			return "Sentry";
		case tftype_object::teleporter:
			return "Teleporter";
		default:
			return "Unknown";
		}
	else if (t == tftype::projectile)
		switch (((tfprojectile*)this)->projectile_type())
		{
		case tftype_projectile::arrow:
			return "Arrow";
		case tftype_projectile::ball:
			return "Ball";
		case tftype_projectile::cleaver:
			return "Cleaver";
		case tftype_projectile::flare:
			return "Flare";
		case tftype_projectile::jar:
			return "Jar";
		case tftype_projectile::pill:
			return "Pill";
		case tftype_projectile::rocket:
			return "Rocket";
		case tftype_projectile::sticky:
			return "Sticky";
		default:
			return "Unknown";
		}
	return "Unknown";
}
bool tfentity::damageable()
{
	auto t = type();
	if (t == tftype::player)
		return true; // TODO: check invincibility status
	else if (t == tftype::projectile)
		// TODO: Check if the sticky is on the ground
		return ((tfprojectile*)this)->projectile_type() == tftype_projectile::sticky;
	else if (t == tftype::object)
		switch (((tfobject*)this)->object_type())
		{
		case tftype_object::dispenser:
		case tftype_object::sentry:
		case tftype_object::teleporter:
			return true;
		default: return false;
		}
	return false;
}

}
