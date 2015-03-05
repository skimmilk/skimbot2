/*
 * bhop.cpp
 *
 *  Created on: Mar 2, 2015
 *      Author: skim
 */

#include "bhop.h"

#include <math.h>
#include "const.h"
#include "tfplayer.h"
#include "basehook.h"
#include "sourceutil.h"
#include "exit.h"
#include "sdk/cvar.h"
#include "sdk/usercmd.h"
#include "sdk/engine.h"

namespace skim
{

ConVar* henable;
ConVar* strafe;
ConVar* samt;

// Last position
Vector last;

static void frame(CUserCmd* cmd)
{
	// Did we just now press the jump button?
	static bool first = true;
	bool grounded = tfplayer::me()->m_fFlags() & FL_ONGROUND;
	bool jumping = cmd->buttons & IN_JUMP;

	if (!jumping)
		first = true;
	else if (first && jumping)
		first = false;
	// If we are not on the ground, disable the jump button
	else if (jumping && henable->m_nValue && !grounded)
		cmd->buttons &= ~IN_JUMP;

	// Auto-strafe
	Vector here = tfplayer::me()->GetAbsOrigin();
	if (!grounded && jumping && strafe->m_nValue && fabsf(cmd->sidemove) > 100.f)
	{
		Vector difference;
		VectorSubtract(here, last, difference);
		difference.z = 0;
		float speed = sqrt(difference.LengthSqr());

		if (speed > 1.)
		{
			float diffangle = samt->m_fValue / speed;
			if (cmd->sidemove > 0.f)
				diffangle *= -1.f;

			cmd->viewangles.y += diffangle;
			ifs::engine->SetViewAngles(cmd->viewangles);
		}
	}
	last = here;
}

static void unload()
{
	delete henable;
	delete strafe;
	delete samt;
}
void bhop::init()
{
	henable = new ConVar(PREFIX "bhop", "0", 0, "Enable bunny hopping");
	strafe = new ConVar(PREFIX "strafe", "0", 0, "Enable auto air strafing");
	samt = new ConVar(PREFIX "strafe_amount", "10", 0, "Autostrafe amount");
	basehook::post_move(frame, "bhop");
	exit::handle(unload);
}

} /* namespace skim */
