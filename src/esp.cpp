/*
 * esp.cpp
 *
 *  Created on: Mar 5, 2015
 *      Author: skim
 */

#include "esp.h"

#include <math.h>
#include <map>
#include "const.h"
#include "basehook.h"
#include "tfplayer.h"
#include "exit.h"
#include "draw.h"
#include "ifs.h"
#include "sourceutil.h"
#include "skeltal.h"
#include "sdk/cvar.h"
#include "sdk/engine.h"

namespace skim
{

static ConVar* enabled;
static ConVar* box;
static ConVar* maxdist;
static ConVar* friendlies; // Draw friendlies
static ConVar* sightdist; // Length of the line-of-sight bar
static ConVar* falloff; // If players are further away than this, don't draw detailed stuff
static ConVar* cursor; // Draw cursor to closest player
static ConVar* draw_objs; // Draw objects (sentries, dispencers, teleporters)
static ConVar* draw_projs; // Draw projectiles
static ConVar* unmask; // Remove spies' disguises
static ConVar* uncloak; // Remove spies' cloaks
static ConVar* glow; // Enable the glow effect on entities
static ConVar* everything; // Draw ESP on all entities, for debugging

static int highlighted_ent; // One of the entities that is being aimed at by the user

// Entities and the colors they need to be drawn with
static std::map<int, color> highlights, hlstage;

static const color& get_highlight(int index, const color& def)
{
	auto it = highlights.find(index);
	if (it == highlights.end())
		return def;
	return it->second;
}

static float dist(tfentity* me, tfentity* them)
{
	Vector diff = me->GetAbsOrigin() - them->GetAbsOrigin();
	return sqrtf(diff.LengthSqr());
}
// Draw box around object
// on_cursor is set to true if the cursor is inside the box
static bool simple_esp(tfentity* them, float width, float height, const color& c,
		point& bottom_left, bool& on_cursor)
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

	if (box->m_nValue)
	{
		draw::line(ptl, ptr, c);
		draw::line(ptr, pbr, c);
		draw::line(pbl, pbr, c);
		draw::line(ptl, pbl, c);
	}

	bottom_left = pbl;

	int x,y;
	ifs::engine->GetScreenSize(x, y);
	x /= 2;
	y /= 2;
	on_cursor = (x >= ptl.x && y >= ptl.y && x <= pbr.x && y <= pbr.y);
	return true;
}
// Draw the ESP on given entity (assumes valid but still does additional checks)
static void draw(tfentity* pl, const color& c, float distance)
{
	point bottom_left;
	float width = 16, height = 85;
	if (pl->type() != tftype::player)
	{
		width = 8;
		height = 16;
	}
	bool on_cursor = false;
	const color& highlight = get_highlight(pl->entindex(), c);
	// Draw simple ESP
	if (!simple_esp(pl, width, height, highlight, bottom_left, on_cursor))
		return;

	// Draw complex ESP if the cursor is hovering over the entity
	if (!on_cursor && distance > falloff->m_fValue)
		return;
	if (on_cursor)
		highlighted_ent = pl->entindex();

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
	auto type = ent->type();
	if (type == tftype::object && !draw_objs->m_nValue)
		return false;
	if (type == tftype::projectile && !draw_projs->m_nValue)
		return false;
	if (type == tftype::other && !everything->m_nValue)
		return false;

	if (type == tftype::player)
		if (!((tfplayer*)ent)->is_drawable())
			return false;

	return	!ent->IsDormant() &&
			(friendlies->m_nValue || ((tfplayer*)ent)->m_iTeamNum() != tfplayer::me()->m_iTeamNum()) &&
			distance < maxdist->m_fValue;
}
// Returns the enemy team color
static color teamcolor(int teamnum)
{
	if (teamnum == 3)
		return {255, 0, 0, 255};
	return {10, 10, 255, 255};
}
// Runs on PaintTraverse
static void paint()
{
	if (!enabled->m_nValue)
		return;

	highlighted_ent = 0;

	float min_dist = 8192;
	tfplayer* me = tfplayer::me();
	tfentity* closest = 0;

	color drawcolor = teamcolor(me->m_iTeamNum());

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
		if (distance < min_dist && pl->damageable())
		{
			min_dist = distance;
			closest = pl;
		}
	}

	// Draw cursor to nearest enemy
	if (cursor->m_nValue && closest)
	{
		point enemy;
		Vector world;

		if (closest->type() == tftype::player)
		{
			world.z += 40;
			world = skeltal::pos((tfplayer*)closest, tfhitbox::HITBOX_HEAD);
		}
		else
			world = closest->GetAbsOrigin();

		int w, h;
		ifs::engine->GetScreenSize(w, h);

		if (draw::world_point(world, enemy))
			draw::line(enemy, {w/2, h/2}, {255,255,255,64});
		else
			draw::line({w/2, h}, {w/2, h/2}, {255,255,255,64});
	}
}

static void unmasker(tfplayer* pl)
{
	if (!(friendlies->m_nValue || pl->m_iTeamNum() != tfplayer::me()->m_iTeamNum()) ||
			pl->m_iClass() != tfclass::spy)
		return;

	if (unmask->m_nValue)
		pl->m_nPlayerCond() &= ~COND_DISGUISED;
	if (uncloak->m_nValue)
		pl->m_nPlayerCond() &= ~COND_CLOAKED;
}
// Update glow effect by either turning it on or off
static void setglow(tfentity* ent)
{
	if (!glow->m_nValue || (ent->type() != tftype::player && ent->type() != tftype::projectile))
		return;

	int team = ((tfplayer*)ent)->m_iTeamNum();
	float distance = dist(ent, tfplayer::me());
	bool wanted = ((friendlies->m_nValue || team != tfplayer::me()->m_iTeamNum()) &&
			distance < falloff->m_fValue) ||
					ent->entindex() == highlighted_ent;
	bool& current = ((tfplayer*)ent)->m_bGlowEnabled();

	if (wanted == current)
		// Nothing to do here
		return;

	current = wanted;

	typedef void (*setglowfn)(tfentity*);
	setglowfn* vmt = *(setglowfn**)ent;
	setglowfn funk;

	// Offset of C_TFPlayer::UpdateGlowEffect is 289
	if (wanted)
		funk = vmt[289]; // UpdateGlowEffect
	else
		funk = vmt[290]; // DestroyGlowEffect
	funk(ent);
}
// ESP that runs on createmove
static void cmesp()
{
	int maxplayers = ifs::engine->GetMaxClients();

	for (int i = ENT_START; i < maxplayers + 1; i++)
	{
		tfentity* pl = (tfentity*)ifs::entities->GetClientEntity(i);
		if (!pl)
			continue;

		unmasker((tfplayer*)pl);
		setglow(pl);
	}
}
// Runs after CreateMove
static void frame(CUserCmd*)
{
	if (!enabled->m_nValue)
		return;
	if (glow->m_nValue || unmask->m_nValue || uncloak->m_nValue)
		cmesp();

	// Clear the highlight stager
	highlights.clear();
}

void esp::highlight(int index)
{
	highlights[index] = {255, 0, 255, 255};
}
void esp::highlight(int index, const color& c)
{
	highlights[index] = c;
}

static void unload()
{
	delete enabled;
	delete box;
	delete maxdist;
	delete friendlies;
	delete sightdist;
	delete falloff;
	delete cursor;
	delete draw_objs;
	delete draw_projs;
	delete unmask;
	delete uncloak;
	delete glow;
	delete everything; // oh no
}
void esp::init()
{
	enabled =	new ConVar(PREFIX "esp", "0");
	box =		new ConVar(PREFIX "esp_box", "1");
	maxdist =	new ConVar(PREFIX "esp_maxdist", "4000");
	friendlies=	new ConVar(PREFIX "esp_friendlies", "0");
	sightdist =	new ConVar(PREFIX "esp_sightdist", "0");
	falloff =	new ConVar(PREFIX "esp_falloff", "1250");
	cursor =	new ConVar(PREFIX "esp_cursor", "1");
	draw_objs =	new ConVar(PREFIX "esp_objs", "1");
	draw_projs=	new ConVar(PREFIX "esp_proj", "1");
	unmask =	new ConVar(PREFIX "esp_unmask", "0", 0, "Remove spies' disguises");
	uncloak =	new ConVar(PREFIX "esp_uncloak", "0", 0, "Remove spies' cloaks");
	glow =		new ConVar(PREFIX "esp_glow", "1", 0, "Use the glow effect with ESP");
	everything= new ConVar(PREFIX "esp_everything", "0");

	basehook::post_paint(paint, "esp");
	basehook::post_move(frame, "esp");
	exit::handle(unload);
}

} /* namespace skim */
