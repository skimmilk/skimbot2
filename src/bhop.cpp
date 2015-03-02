/*
 * bhop.cpp
 *
 *  Created on: Mar 2, 2015
 *      Author: skim
 */

#include "bhop.h"

#include "const.h"
#include "tfplayer.h"
#include "basehook.h"
#include "sdk/cvar.h"
#include "sdk/usercmd.h"

namespace skim
{

ConVar* henable;

static void frame(CUserCmd* cmd)
{
	// Did we just now press the jump button?
	static bool first = true;
	if (first && cmd->buttons & IN_JUMP)
		first = false;
	// If we are not on the ground, disable the jump button
	else if (cmd->buttons & IN_JUMP && henable->m_nValue && !(tfplayer::me()->m_fFlags() & FL_ONGROUND))
		cmd->buttons &= ~IN_JUMP;
}

void bhop::init()
{
	henable = new ConVar(PREFIX "bhop", "0", 0, "Enable bunny hopping");
	basehook::post_move(frame, "bhop");
}
void bhop::unload()
{
	delete henable;
}

} /* namespace skim */
