/*
 * nospread.cpp
 *
 *  Created on: Apr 28, 2015
 *      Author: skim
 */

#include "nospread.h"

#include "ifs.h"
#include "basehook.h"
#include "exit.h"
#include "const.h"
#include "tfplayer.h"
#include "trig.h"
#include "sdk/engine.h"
#include "sdk/cvar.h"

#include "debug.h"

namespace skim
{

ConVar* spread_enable;

float get_spread(tfweapon* wep)
{
	// Call the virtual GetWeaponSpread method
	typedef float (*get_spread_fn)(tfweapon*);
	get_spread_fn* vmt = *(get_spread_fn**)wep;
	return vmt[518](wep);
}
float normal_angle(float a)
{
	while (a <= -180.f) a += 360;
	while (a > 180.f) a -= 360;
	return a;
}
void spread_frame(CUserCmd* cmd)
{
	tfweapon* wep = tfplayer::me()->weapon();
	if (!(cmd->buttons & IN_ATTACK) || !spread_enable->m_nValue ||
			!wep || wep->slot() == tfslot::melee)
		return;

	ifs::RandomSeed(cmd->random_seed & 0xff);

	float x = ifs::RandomFloat(-.5f, .5f) + ifs::RandomFloat(-.5f, .5f);
	float y = ifs::RandomFloat(-.5f, .5f) + ifs::RandomFloat(-.5f, .5f);
	float mult = get_spread(wep);

	Vector offset {x * mult, y * mult, 0};
	Vector forward, right, up;
	QAngle result;

	AngleVectors(cmd->viewangles, &forward, &right, &up);

	Vector newang = forward + (right * offset.x) + (up * offset.y);
	VectorNormalize(newang);
	VectorAngles(newang, result);

	result.x = normal_angle(cmd->viewangles.x*2 - result.x);
	result.y = normal_angle(cmd->viewangles.y*2 - result.y);

	trig::moveangle(cmd, result.y);
	cmd->viewangles = result;
}

void spread_exit()
{
	delete spread_enable;
}
void nospread::init()
{
	spread_enable = new ConVar(PREFIX "nospread_auto", "0", 0, "Enable nospread");

	basehook::post_move(spread_frame, "nospread");
	exit::handle(spread_exit);
}

} /* namespace skim */
