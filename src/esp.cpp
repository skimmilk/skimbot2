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
static void simple_esp(tfplayer* them, float distance)
{
	Vector mins = them->GetAbsOrigin();
	Vector maxs = mins;
	maxs.z += 80;
	mins.z -= 5;

	int scrwidth = width->m_fValue / distance;
	point tl, br;

	if (!draw::world_point(mins, br) ||
			!draw::world_point(maxs, tl))
		return;

	tl.x -= scrwidth;
	br.x += scrwidth;

	draw::line_box(tl, br);
}
// Draw the ESP on given player (assumes player is valid but still does additional checks)
static void draw(tfplayer* pl, float distance)
{
	// Draw simple ESP
	simple_esp(pl, distance);

	if (distance > falloff->m_fValue)
		return;
}
static void paint()
{
	if (!esp_enabled->m_nValue)
		return;

	int end = ifs::engine->GetMaxClients() + 1;
	float max_dist = maxdist->m_fValue;
	tfplayer* me = tfplayer::me();

	color drawcolor {10, 10, 255, 255};
	if (me->m_iTeamNum() == 3)
		drawcolor = {255, 0, 0, 255};
	draw::color(drawcolor);

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
			draw(pl, distance);
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
}
void esp::init()
{
	esp_enabled=new ConVar(PREFIX "esp", "0");
	width =		new ConVar(PREFIX "esp_width", "1000");
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
