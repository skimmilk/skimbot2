/*
 * tfplayer.cpp
 *
 *  Created on: Mar 6, 2015
 *      Author: skim
 */

#include "tfplayer.h"

#include <unordered_map>
#include "ifs.h"
#include "const.h"
#include "sourceutil.h"
#include "sdk/engine.h"

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

// Register weapon and slot
#define REGWEP(str, slot) classid[netvar::classid<_id>(#str)] = (tfslot)slot;

// First parameter is class ID, second slot
std::unordered_map<int, tfslot> classid;

static void init()
{
	REGWEP(CTFWearableRobotArm, 3);
	REGWEP(CTFRobotArm, 3);
	REGWEP(CTFWrench, 3);
	REGWEP(CTFSyringeGun, 1);
	REGWEP(CTFKatana, 3);
	REGWEP(CTFSword, 3);
	REGWEP(CTFSniperRifleClassic, 1);
	REGWEP(CTFSniperRifleDecap, 1);
	REGWEP(CTFSniperRifle, 1);
	REGWEP(CTFSMG, 2);
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
	REGWEP(CTFPistol_ScoutPrimary, 2);
	REGWEP(CTFPistol_Scout, 2);
	REGWEP(CTFPistol, 2);
	REGWEP(CTFPipebombLauncher, 2);
	REGWEP(CTFWeaponPDA_Spy, 4);
	REGWEP(CTFWeaponPDA_Engineer_Destroy, 4);
	REGWEP(CTFWeaponPDA_Engineer_Build, 4);
	REGWEP(CTFWeaponPDAExpansion_Teleporter, 4);
	REGWEP(CTFWeaponPDAExpansion_Dispenser, 4);
	REGWEP(CTFWeaponPDA, 4);
	REGWEP(CTFParticleCannon, 1);
	REGWEP(CTFMinigun, 1);
	REGWEP(CTFMedigunShield, 2);
	REGWEP(CWeaponMedigun, 2);
	REGWEP(CTFMechanicalArm, 3);
	REGWEP(CTFLunchBox_Drink, 2);
	REGWEP(CTFLunchBox, 2);
	REGWEP(CTFKnife, 3);
	REGWEP(CTFCleaver, 3);
	REGWEP(CTFJarMilk, 2);
	REGWEP(CTFJar, 2);
	REGWEP(CTFWeaponInvis, 4);
	REGWEP(CTFCannon, 1);
	REGWEP(CTFGrenadeLauncher, 1);
	REGWEP(CTFFlareGun_Revenge, 2);
	REGWEP(CTFFlareGun, 2);
	REGWEP(CTFFlameRocket, 1);
	REGWEP(CTFFlameThrower, 1);
	REGWEP(CTFFists, 3);
	REGWEP(CTFFireAxe, 3);
	REGWEP(CTFCompoundBow, 1);
	REGWEP(CTFClub, 3);
	REGWEP(CTFBuffItem, 2);
	REGWEP(CTFStickBomb, 2);
	REGWEP(CTFBottle, 3);
	REGWEP(CTFBonesaw, 3);
	REGWEP(CTFBall_Ornament, 3);
	REGWEP(CTFStunBall, 3);
	REGWEP(CTFBat_Giftwrap, 3);
	REGWEP(CTFBat_Wood, 3);
	REGWEP(CTFBat_Fish, 3);
	REGWEP(CTFBat, 3);
	REGWEP(CTFWeaponBuilder, 2);
}
tfslot tfweapon::slot()
{
	static bool initialized = false;
	if (!initialized)
	{
		init();
		initialized = true;
	}

	auto needle = classid.find(this->GetClientClass()->m_ClassID);
	if (needle == classid.end())
	{
		econ(NAME "Could not find the weapon slot for weapon " +
				std::string(this->GetClientClass()->m_pNetworkName));
		return tfslot::primary;
	}
	return needle->second;
}

}
