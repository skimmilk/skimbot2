/*
 * trigger.cpp
 *
 *  Created on: Mar 1, 2015
 *      Author: skim
 */

#include "trigger.h"

#include "const.h"
#include "basehook.h"
#include "tfplayer.h"
#include "trace.h"
#include "tfdebug.h"
#include "sdk/cvar.h"

namespace skim
{

ConVar* enabled;
ConCommand* contfdebug;
bool do_debug;

// Returns the index of the player in sights
static int enemy(CUserCmd* cmd)
{
	tfplayer* me = tfplayer::me();
	tfplayer* sight = trace::sight(me->local_eyes(), cmd->viewangles, ifs::engine->GetLocalPlayer());

	if (!sight)
		return 0;

	if (sight->m_iTeamNum() > 1 && sight->m_iTeamNum() != me->m_iTeamNum())
	{
		return sight->entindex();
	}
	return 0;
}

static void frame(CUserCmd* cmd)
{
	if (enabled->m_nValue && enemy(cmd))
	{
		cmd->buttons |= IN_ATTACK;
		cmd->buttons &= ~IN_RELOAD;
	}

	if (do_debug)
	{
		do_debug = false;
		tfdebug::playerinfo(enemy(cmd));
	}
}

void trigger::init()
{
	enabled = new ConVar(PREFIX "trigger_auto", "0", 0, "Enable or disable the triggerbot");
	contfdebug = new ConCommand(PREFIX "trigger_debug",
			[](){do_debug = true;}, "Get information of the player pointed at");
	basehook::post_move(frame, "trigger");
}

void trigger::unload()
{
	delete enabled;
	delete contfdebug;
}

} /* namespace skim */
