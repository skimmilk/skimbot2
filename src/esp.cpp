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
ConVar* width;

static float dist(tfplayer* me, tfplayer* them)
{
	Vector diff = me->GetAbsOrigin() - them->GetAbsOrigin();
	return sqrtf(diff.LengthSqr());
}
// Draw box around player
static bool simple_esp(tfplayer* them, color c, point& bottom_left)
{
	Vector mins = them->GetAbsOrigin();
	Vector maxs = mins;
	maxs.z += 80;
	mins.z -= 5;

	float width = 16;
	Vector dir_left;
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
// Draw the ESP on given player (assumes player is valid but still does additional checks)
static void draw(tfplayer* pl, color c, float distance)
{
	point bottom_left;
	// Draw simple ESP
	if (!simple_esp(pl, c, bottom_left))
		return;

	if (distance > falloff->m_fValue)
		return;

	player_info_t info;
	ifs::engine->GetPlayerInfo(pl->entindex(), &info);
	bottom_left.y += 10;
	draw::string(bottom_left, c, info.name);

	bottom_left.y += 10;
	draw::string(bottom_left, c, "Health: " + std::to_string(pl->m_iHealth()));
}
static void paint()
{
	if (!esp_enabled->m_nValue)
		return;

	int end = ifs::engine->GetMaxClients() + 1;
	float max_dist = maxdist->m_fValue;
	float min_dist = 8192;
	tfplayer* me = tfplayer::me();
	tfplayer* closest = 0;

	color drawcolor {10, 10, 255, 255};
	if (me->m_iTeamNum() == 3)
		drawcolor = {255, 0, 0, 255};

	for (int i = 1; i < end; i++)
	{
		if (i == ifs::engine->GetLocalPlayer())
			continue;

		tfplayer* pl = (tfplayer*)ifs::entities->GetClientEntity(i);
		if (!pl)
			continue;

		float distance = dist(me, pl);

		if (pl->is_drawable() &&
				(friendlies->m_nValue || pl->m_iTeamNum() != me->m_iTeamNum()) &&
				distance < max_dist)
		{
			draw(pl, drawcolor, distance);
			if (distance < min_dist)
			{
				min_dist = distance;
				closest = pl;
			}
		}
	}

	// Draw cursor to nearest enemy
	if (cursor->m_nValue && closest)
	{
		point enemy;
		Vector world = closest->GetAbsOrigin();
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
}
void esp::init()
{
	esp_enabled=new ConVar(PREFIX "esp", "0");
	width =		new ConVar(PREFIX "esp_width", "10000");
	box =		new ConVar(PREFIX "esp_box", "1");
	bones =		new ConVar(PREFIX "esp_bones", "0");
	maxdist =	new ConVar(PREFIX "esp_maxdist", "8192");
	friendlies=	new ConVar(PREFIX "esp_friendlies", "0");
	sightdist =	new ConVar(PREFIX "esp_sightdist", "0");
	falloff =	new ConVar(PREFIX "esp_falloff", "3000");
	cursor =	new ConVar(PREFIX "esp_cursor", "1");

	basehook::post_paint(paint, "esp");
	exit::handle(unload);
}

} /* namespace skim */
