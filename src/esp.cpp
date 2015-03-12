/*
 * esp.cpp
 *
 *  Created on: Mar 5, 2015
 *      Author: skim
 */

#include "esp.h"

#include <math.h>
#include "const.h"
#include "basehook.h"
#include "tfplayer.h"
#include "exit.h"
#include "draw.h"
#include "ifs.h"
#include "sourceutil.h"
#include "sdk/cvar.h"
#include "sdk/engine.h"

namespace skim
{

ConVar* esp_enabled;
ConVar* box;
ConVar* bones;
ConVar* glow;
ConVar* maxdist;
ConVar* friendlies; // Draw friendlies
ConVar* sightdist; // Length of the line-of-sight bar
ConVar* falloff; // If players are further away than this, don't draw detailed stuff
ConVar* cursor; // Draw cursor to closest player
ConVar* draw_objs; // Draw objects (sentries, dispencers, teleporters)
ConVar* draw_projs; // Draw projectiles

static float dist(tfentity* me, tfentity* them)
{
	Vector diff = me->GetAbsOrigin() - them->GetAbsOrigin();
	return sqrtf(diff.LengthSqr());
}
// Draw box around object
static bool simple_esp(tfentity* them, float width, float height, color c, point& bottom_left)
{
	Vector mins = them->GetAbsOrigin();
	Vector maxs = mins;
	maxs.z += height - 5;
	mins.z -= 5;

	Vector dir_right;
	QAngle perpendicular;

	ifs::engine->GetViewAngles(perpendicular);
	perpendicular.y -= 90.f;
	perpendicular.x = 0;

	dir_right = AngleVectors(perpendicular);
	dir_right *= width;

	// Top left, top right, ...
	Vector vtl, vtr, vbl, vbr;
	vtl = maxs - dir_right;
	vtr = maxs + dir_right;
	vbl = mins - dir_right;
	vbr = mins + dir_right;

	point ptl, ptr, pbl, pbr;
	if (!draw::world_point(vtl, ptl) ||
			!draw::world_point(vtr, ptr) ||
			!draw::world_point(vbl, pbl) ||
			!draw::world_point(vbr, pbr))
		return false;

	draw::line(ptl, ptr, c);
	draw::line(ptr, pbr, c);
	draw::line(pbl, pbr, c);
	draw::line(ptl, pbl, c);

	bottom_left = pbl;
	return true;
}
// Draw the ESP on given entity (assumes valid but still does additional checks)
static void draw(tfentity* pl, color c, float distance)
{
	point bottom_left;
	float width = 16, height = 85;
	if (pl->type() != tftype::player)
	{
		width = 8;
		height = 16;
	}
	// Draw simple ESP
	if (!simple_esp(pl, width, height, c, bottom_left))
		return;

	if (distance > falloff->m_fValue)
		return;

	int fontheight = 10;
	color white {255,255,255,255};

	// Print type of object
	draw::string(bottom_left, white, pl->type_name());

	// Draw health
	if (pl->type() == tftype::object && pl->damageable())
	{
		int health = ((tfobject*)pl)->m_iHealth();
		if (health)
		{
			bottom_left.y += fontheight;
			draw::string(bottom_left, white, "Health: " + std::to_string(health));
		}
	}

	// Draw player information: name and weapon
	if (pl->type() == tftype::player)
	{
		bottom_left.y += fontheight;
		draw::string(bottom_left, white, "Health: " + std::to_string(((tfplayer*)pl)->m_iHealth()));

		player_info_t info;
		ifs::engine->GetPlayerInfo(pl->entindex(), &info);

		bottom_left.y += fontheight;
		draw::string(bottom_left, white, info.name);

		// Draw weapon information
		tfweapon* wep = ((tfplayer*)pl)->weapon();
		if (wep)
		{
			bottom_left.y += fontheight;
			draw::string(bottom_left, white, wep->GetClientClass()->m_pNetworkName);
		}
	}
}
static bool can_draw(tfentity* ent, float distance)
{
	if (ent->type() == tftype::object && !draw_objs->m_nValue)
		return false;
	if (ent->type() == tftype::projectile && !draw_projs->m_nValue)
		return false;
	if (ent->type() == tftype::other)
		return false;

	if (ent->type() == tftype::player)
		if (!((tfplayer*)ent)->is_drawable())
			return false;

	return	!ent->IsDormant() &&
			ent->type() != tftype::other &&
			(friendlies->m_nValue || ((tfplayer*)ent)->m_iTeamNum() != tfplayer::me()->m_iTeamNum()) &&
			distance < maxdist->m_fValue;
}
static void paint()
{
	if (!esp_enabled->m_nValue)
		return;

	float min_dist = 8192;
	tfplayer* me = tfplayer::me();
	tfentity* closest = 0;

	color drawcolor {10, 10, 255, 255};
	if (me->m_iTeamNum() == 3)
		drawcolor = {255, 0, 0, 255};

	// Loop over every networked entity
	for (int i = 1; i < ENT_MAX; i++)
	{
		if (i == ifs::engine->GetLocalPlayer())
			continue;

		tfentity* pl = (tfentity*)ifs::entities->GetClientEntity(i);
		if (!pl)
			continue;

		float distance = dist(me, pl);
		if (!can_draw(pl, distance))
			continue;

		draw(pl, drawcolor, distance);

		// Set closest entity
		if (pl->damageable() && distance < min_dist)
		{
			min_dist = distance;
			closest = pl;
		}
	}

	// Draw cursor to nearest enemy
	if (cursor->m_nValue && closest)
	{
		point enemy;
		Vector world = closest->GetAbsOrigin();

		if (closest->type() == tftype::player)
			world.z += 40;
		int w, h;
		ifs::engine->GetScreenSize(w, h);

		if (draw::world_point(world, enemy))
			draw::line(enemy, {w/2, h/2}, {255,255,255,64});
		else
			draw::line({w/2, h}, {w/2, h/2}, {255,255,255,64});
	}
}

static void unload()
{
	delete esp_enabled;
	delete box;
	delete bones;
	delete maxdist;
	delete friendlies;
	delete sightdist;
	delete falloff;
	delete cursor;
	delete draw_objs;
	delete draw_projs;
}
void esp::init()
{
	esp_enabled=new ConVar(PREFIX "esp", "0");
	box =		new ConVar(PREFIX "esp_box", "1");
	bones =		new ConVar(PREFIX "esp_bones", "0");
	maxdist =	new ConVar(PREFIX "esp_maxdist", "4000");
	friendlies=	new ConVar(PREFIX "esp_friendlies", "0");
	sightdist =	new ConVar(PREFIX "esp_sightdist", "0");
	falloff =	new ConVar(PREFIX "esp_falloff", "1250");
	cursor =	new ConVar(PREFIX "esp_cursor", "1");
	draw_objs =	new ConVar(PREFIX "esp_objs", "1");
	draw_projs=	new ConVar(PREFIX "esp_proj", "1");

	basehook::post_paint(paint, "esp");
	exit::handle(unload);
}

} /* namespace skim */
