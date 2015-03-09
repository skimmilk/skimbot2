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
#include "exit.h"
#include "ifs.h"
#include "sdk/engine.h"
#include "sdk/cvar.h"

namespace skim
{

ConVar* enabled;
ConVar* hit_objs;
ConCommand* contfdebug;
bool do_debug;

static bool enemy(CUserCmd* cmd)
{
	tfplayer* me = tfplayer::me();
	tfentity* sight = trace::sight(me->local_eyes(), cmd->viewangles, ifs::engine->GetLocalPlayer());

	if (!sight && sight->entindex() < 1)
		return 0;

	switch (sight->type())
	{
	case tftype::player:
		return ((tfplayer*)sight)->m_iTeamNum() != me->m_iTeamNum();
	case tftype::object:
		return hit_objs->m_nFlags && ((tfobject*)sight)->m_iTeamNum() != me->m_iTeamNum() &&
				(((tfobject*)sight)->object_type() == tftype_object::sentry ||
				((tfobject*)sight)->object_type() == tftype_object::dispenser ||
				((tfobject*)sight)->object_type() == tftype_object::teleporter);
	default:
		return 0;
	}
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
		tfdebug::playerinfo(trace::sight(tfplayer::me()->local_eyes(), cmd->viewangles,
				ifs::engine->GetLocalPlayer())->entindex());
	}
}

static void unload()
{
	delete enabled;
	delete contfdebug;
	delete hit_objs;
}
void trigger::init()
{
	enabled = new ConVar(PREFIX "trigger_auto", "0", 0, "Enable or disable the triggerbot");
	hit_objs = new ConVar(PREFIX "trigger_objects", "1", 0, "Objects ");
	contfdebug = new ConCommand(PREFIX "trigger_debug",
			[](){do_debug = true;}, "Get information of the player pointed at");
	basehook::post_move(frame, "trigger");
	exit::handle(unload);
}

} /* namespace skim */
