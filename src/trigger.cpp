/*
 * trigger.cpp
 *
 *  Created on: Mar 1, 2015
 *      Author: skim
 */

#include "trigger.h"

#include <math.h>
#include "const.h"
#include "basehook.h"
#include "tfplayer.h"
#include "trace.h"
#include "tfdebug.h"
#include "exit.h"
#include "ifs.h"
#include "esp.h"
#include "sdk/engine.h"
#include "sdk/cvar.h"

namespace skim
{

ConVar* enabled;
ConVar* hit_objs;
ConVar* reflect;
ConVar* reflect_dist;
ConVar* reflect_fov;
ConCommand* contfdebug;
bool do_debug;

// Returns the enemy we're looking at
static tfentity* enemy(CUserCmd* cmd)
{
	tfplayer* me = tfplayer::me();
	tfentity* sight = trace::sight(me->local_eyes(), cmd->viewangles, ifs::engine->GetLocalPlayer());

	if (!sight && sight->entindex() < 1)
		return 0;

	switch (sight->type())
	{
	case tftype::player:
		return (((tfplayer*)sight)->m_iTeamNum() != me->m_iTeamNum()) ? sight : 0;
	case tftype::object:
		return (hit_objs->m_nFlags && ((tfobject*)sight)->m_iTeamNum() != me->m_iTeamNum() &&
				sight->damageable()) ? sight : 0;
	default:
		return 0;
	}
}
// Determines if our weapon can do damage at this distance from player
static bool can_fire(tfentity* en)
{
	tfweapon* wep = tfplayer::me()->weapon();
	if (!wep || !wep->damaging())
		return false;

	if (!en->damageable())
		return false;

	// Don't enable triggerbot with knife
	if (tfplayer::me()->m_iClass() == tfclass::spy && wep->slot() == tfslot::melee)
		return false;
	if (wep->slot() == tfslot::melee)
		return sqrtf((tfplayer::me()->GetAbsOrigin() - en->GetAbsOrigin()).LengthSqr()) < 128;
	return true;
}
static void triggerbot(CUserCmd* cmd)
{
	tfplayer* me = tfplayer::me();
	if (!me->weapon())
		return;

	tfentity* en = 0;
	// Knifebot
	if (me->m_iClass() == tfclass::spy &&
			me->weapon()->slot() == tfslot::melee && me->weapon()->m_bReadyToBackstab())
		cmd->buttons |= IN_ATTACK;
	else if ((en = enemy(cmd)))
	{
		if (can_fire(en))
			cmd->buttons |= IN_ATTACK;
		// Highlight player to show we are eyeing it
		esp::highlight(en->entindex());
	}
}
static void reflectbot(CUserCmd* cmd)
{
	tfplayer* me = tfplayer::me();
	if (me->m_iClass() != tfclass::pyro ||
			!me->weapon() ||
			me->weapon()->slot() != tfslot::primary)
		return;

	int current_team = me->m_iTeamNum();
	Vector eyepos = me->local_eyes();

	for (int i = ENT_START; i < ENT_MAX; i++)
	{
		tfprojectile* ent = (tfprojectile*)ifs::entities->GetClientEntity(i);
		if (!ent || ent->type() != tftype::projectile || ent->m_iTeamNum() == current_team)
			continue;

		float distance = sqrtf((eyepos - ent->GetAbsOrigin()).LengthSqr());
		if (distance > reflect_dist->m_fValue)
			continue;

		// Get angle difference
		QAngle toent, eyes;
		VectorAngles(ent->GetAbsOrigin() - me->local_eyes(), toent);
		ifs::engine->GetViewAngles(eyes);
		float d1 = 180.f - fabsf(fabsf(toent.x - eyes.x) - 180.f);
		float d2 = 180.f - fabsf(fabsf(toent.y - eyes.y) - 180.f);
		if (d1 + d2 > reflect_fov->m_fValue)
			continue;

		cmd->buttons |= IN_ATTACK2;
		break;
	}
}

static void frame(CUserCmd* cmd)
{
	if (enabled->m_nValue)
		triggerbot(cmd);
	if (reflect->m_nValue)
		reflectbot(cmd);

	if (do_debug)
	{
		do_debug = false;
		tfdebug::playerinfo(trace::sight(tfplayer::me()->local_eyes(), cmd->viewangles,
				ifs::engine->GetLocalPlayer())->entindex());
	}
}

static void unload()
{
	delete enabled;
	delete contfdebug;
	delete hit_objs;
	delete reflect;
	delete reflect_dist;
	delete reflect_fov;
}
void trigger::init()
{
	enabled = new ConVar(PREFIX "trigger_auto", "0", 0, "Enable or disable the triggerbot");
	hit_objs = new ConVar(PREFIX "trigger_objects", "1", 0, "Objects get hit");
	reflect = new ConVar(PREFIX "trigger_reflect", "1", 0, "Reflect projectiles");
	reflect_fov = new ConVar(PREFIX "trigger_reflect_fov", "40", 0, "FOV reflectbot has");
	reflect_dist = new ConVar(PREFIX "trigger_reflect_distance", "256", 0, "Maximum distance to reflect projectile");
	contfdebug = new ConCommand(PREFIX "trigger_debug",
			[](){do_debug = true;}, "Get information of the player pointed at");
	basehook::post_move(frame, "trigger");
	exit::handle(unload);

	ifs::engine->ClientCmd_Unrestricted("alias +st_trigger \"st_trigger_auto 1\";"
			"alias -st_trigger \"st_trigger_auto 0\"");
}

} /* namespace skim */
