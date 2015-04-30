/*
 * aim.cpp
 *
 *  Created on: Mar 17, 2015
 *      Author: skim
 */

#include "aim.h"

#include <vector>
#include <algorithm>

#include "ifs.h"
#include "const.h"
#include "basehook.h"
#include "tfplayer.h"
#include "draw.h"
#include "trace.h"
#include "trig.h"
#include "skeltal.h"
#include "exit.h"
#include "sdk/engine.h"
#include "sdk/cvar.h"

namespace skim
{

ConVar* aim_enabled;
ConVar* aim_esp;
ConVar* aim_shotgun_falloff;
ConVar* aim_silent;

Vector aim_point;
bool aiming;

// To decide who to shoot, pick most prioritized entity
struct tfrank {
	float priority;
	tfentity* ent;
	Vector aimpoint;
	bool operator <(const tfrank& l) const
	{
		return priority < l.priority;
	}
};

// Get the ranking multiplier for entities
// -1 = invalid, 1 = normal, bigger = less
static float base_multiplier(tfentity* ent, tftype type, float distance)
{
	switch (type)
	{
	case tftype::player: return 1;
	case tftype::projectile:
		if (((tfprojectile*)ent)->projectile_type() == tftype_projectile::sticky)
		{
			// Prioritize stickies that are close
			if (distance < 300)
				return 1;
			if (distance < 700)
				return 10;
			return -1;
		}
		return -1;
	case tftype::object:
		switch (((tfobject*)ent)->object_type())
		{
		case tftype_object::ammo: case tftype_object::health:
		case tftype_object::money: case tftype_object::other:
			return -1;
		case tftype_object::dispenser: case tftype_object::teleporter:
			return 100;
		case tftype_object::sentry:
			return 1;
		}
		default: return -1;
	}
}
static void get_aim_pos(tfentity* them, tftype type, tfplayer* me, Vector& result)
{
	if (type == tftype::player)
	{
		// Aim for the chest, just like the good guys do in the movies
		tfhitbox hbox = tfhitbox::HITBOX_SPINE_2;

		// If using sniper rifle, aim for head
		if (me->m_iClass() == tfclass::sniper &&
				me->weapon()->slot() == tfslot::primary)
			hbox = tfhitbox::HITBOX_HEAD;

		result = skeltal::pos((tfplayer*)them, hbox);
	}
	else if (type == tftype::object)
	{
		result = them->GetAbsOrigin();
		result.z += 16;
	}
	else
		result = them->GetAbsOrigin();
}
// Check if the shot is too long for weapon
static bool valid_distance(float dist, tfplayer* me, tfslot slot)
{
	if (dist > 8192)
		return false;

	// Determine if we have a shotgun
	auto cl = me->m_iClass();
	if ((slot == tfslot::primary &&
					(cl == tfclass::scout || cl == tfclass::engineer)) ||
			(slot == tfslot::secondary &&
					(cl == tfclass::soldier || cl == tfclass::pyro || cl == tfclass::heavy)))
		return dist < aim_shotgun_falloff->m_fValue;
	// Determine if we have flamethrower
	if (cl == tfclass::pyro && slot == tfslot::primary)
		return dist < 400.f;
	return true;
}
// Checks if this is a valid entity for the weapon we're using
static bool valid_ent(tfentity* ent, tftype type, tfplayer* me, tfslot slot)
{
	// Sanity checks
	if (type == tftype::other || ent->IsDormant())
		return false;
	if (type == tftype::player &&
			!((tfplayer*)ent)->is_alive())
		return false;
	if (((tfplayer*)ent)->m_iTeamNum() == me->m_iTeamNum())
		return false;

	// Could we damage it?
	auto cl = me->m_iClass();
	return !(type == tftype::projectile &&
			(slot == tfslot::melee || cl == tfclass::demoman ||
					(slot == tfslot::primary && (cl == tfclass::pyro || cl == tfclass::soldier))));
}
static void populate_ranks(std::vector<tfrank>& ranks)
{
	tfplayer* me = tfplayer::me();
	tfslot slot = me->weapon()->slot();
	Vector eyes = me->local_eyes();

	for (int i = ENT_START; i < ENT_MAX; i++)
	{
		tfentity* ent = (tfentity*)ifs::entities->GetClientEntity(i);
		if (!ent)
			continue;
		auto type = ent->type();

		if (!valid_ent(ent, type, me, slot))
			continue;

		float distance = sqrtf((eyes - ent->GetAbsOrigin()).LengthSqr());
		if (!valid_distance(distance, me, slot))
			continue;

		float mult = base_multiplier(ent, type, distance);
		if (mult < 0)
			continue;

		// Determine if line-of-sight established
		Vector aimpos;
		get_aim_pos(ent, type, me, aimpos);
		if (trace::can_see_fast(eyes, aimpos, ent))
			ranks.push_back({distance * mult, ent, aimpos});
	}
}
static void aim_frame(CUserCmd* cmd)
{
	aiming = false;
	if (!aim_enabled->m_nValue || !tfplayer::me()->weapon() ||
			!tfplayer::me()->weapon()->damaging())
		return;

	// The ranking system
	// Use static vector to avoid re-allocating buffer every frame
	static std::vector<tfrank> ranks;
	ranks.clear();

	populate_ranks(ranks);

	if (ranks.size() == 0)
		return;

	Vector& at = std::min_element(ranks.begin(), ranks.end())->aimpoint;

	trig::aim(cmd, tfplayer::me()->local_eyes(), at, aim_silent->m_nValue);
	if (!aim_silent->m_nValue)
		ifs::engine->SetViewAngles(cmd->viewangles);
	cmd->buttons |= IN_ATTACK;

	aiming = true;
	aim_point = at;
}
static void aim_paint()
{
	if (!aiming || !aim_esp->m_nValue || !aim_silent->m_nValue)
		return;

	point mid;
	color c {255,0,0,32};
	int w, h;
	ifs::engine->GetScreenSize(w, h);

	if (!draw::world_point(aim_point, mid))
	{
		draw::line({w/2+1, h}, {w/2+1, h/2}, c);
		return;
	}

	// Draw cursor
	point start = mid, end = mid;
	start.y += 8;
	end.y -= 8;
	draw::line(start, end, c);

	start = mid; end = mid;
	start.x -= 8;
	end.x += 8;
	draw::line(start, end, c);

	// Draw line to cursor
	draw::line(mid, {w/2+1, h/2}, c);
}

static void aim_unload()
{
	delete aim_enabled;
	delete aim_esp;
	delete aim_shotgun_falloff;
	delete aim_silent;
	aiming = false;
}
void aim::init()
{
	aim_enabled = new ConVar(PREFIX "aim_auto", "0", 0, "Enable aimbot");
	aim_esp = new ConVar(PREFIX "aim_esp", "1", 0, "Show what aimbot is aiming at");
	aim_shotgun_falloff = new ConVar(PREFIX "aim_shotgun_falloff", "1000", 0,
			"Shotgun range falloff for shotgun aimbot");
	aim_silent = new ConVar(PREFIX "aim_silent", "1", 0, "Use silent aim");

	ifs::engine->ClientCmd_Unrestricted("alias +st_aim \"st_aim_auto 1\";"
			"alias -st_aim \"st_aim_auto 0\"");

	basehook::post_move(aim_frame, "aimbot");
	basehook::post_paint(aim_paint, "aimbot");
	exit::handle(aim_unload);
}

} /* namespace skim */
