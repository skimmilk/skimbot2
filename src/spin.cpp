/*
 * spin.cpp
 *
 *  Created on: Mar 4, 2015
 *      Author: skim
 */

#include "spin.h"

#include "const.h"
#include "basehook.h"
#include "trig.h"
#include "exit.h"
#include "sdk/cvar.h"
#include "sdk/usercmd.h"

namespace skim
{

static ConVar* spin_enabled;
static ConVar* spin_amount;
static ConVar* spin_rand;
static ConVar* spin_aa;

static void frame(CUserCmd* cmd)
{
	if (cmd->buttons & IN_ATTACK  || cmd->buttons & IN_ATTACK2 || !spin_enabled->m_nValue)
		return;

	if (spin_aa->m_nValue)
	{
		cmd->viewangles.x *= -1;
		cmd->viewangles.x += 180;
		cmd->forwardmove *= -1;
	}
	else if (spin_rand->m_nValue)
	{
		QAngle spin;
		// +- 80 degrees
		unsigned int rand = cmd->random_seed * 123456789;
		spin.x = rand % 160 - 80;
		// +- 180 degrees
		spin.y = (rand * 7) % 360 - 180;
		spin.z = 0;

		trig::moveangle(cmd, spin.y);
		cmd->viewangles = spin;
	}
	else
	{
		float spin = (spin_amount->m_nValue * cmd->tick_count) % 360 - 180;
		trig::moveangle(cmd, spin);
		cmd->viewangles.y = spin;
	}
}

static void unload()
{
	delete spin_enabled;
	delete spin_amount;
	delete spin_rand;
	delete spin_aa;
}
void spin::init()
{
	spin_enabled = new ConVar(PREFIX "spin", "0", 0, "Enable spinbot");
	spin_amount = new ConVar(PREFIX "spin_factor", "10", 0, "Degrees turned per tick");
	spin_rand = new ConVar(PREFIX "spin_rand", "0", 0, "Randomly set your spin angles");
	spin_aa = new ConVar(PREFIX "spin_aa", "0", 0, "Reverse your hitbox");

	basehook::post_move(frame, "spinbot");
	exit::handle(unload);
}

} /* namespace skim */
