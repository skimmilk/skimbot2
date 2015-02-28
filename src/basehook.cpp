/*
 * basehook.cpp
 *
 *  Created on: Feb 27, 2015
 *      Author: skim
 */

#include "basehook.h"

#include <vector>
#include <string.h>
#include "ifs.h"
#include "sourceutil.h"
#include "const.h"
#include "vthook.h"
#include "sdk/client.h"
#include "sdk/engine.h"
#include "sdk/panel.h"
#include "sdk/usercmd.h"
#include "sdk/input.h"

namespace skim
{

struct premove_hack { premove_fn fn; std::string name; };
struct postmove_hack { postmove_fn fn; std::string name; };
struct paint_hack { paint_fn fn; std::string name; };

template <typename hack_t>
std::vector<hack_t>& gethacks()
{
	static std::vector<hack_t> h;
	return h;
}

template <typename hack_t, typename... Args>
static void runhacks(const std::string& description, Args&... args)
{
	const std::string* hackerror = nullptr;

	try
	{
		for (const auto& hack : gethacks<hack_t>())
		{
			hackerror = &hack.name;
			hack.fn(args...);
		}
	}
	catch (...)
	{
		econ(NAME "Hack named " + *hackerror + " in " +
				description + " crashed while executing");

		// Show the console
		static int bitchcount;
		if (bitchcount++ < 10)
			skim::ifs::engine->ClientCmd_Unrestricted("showconsole");
	}
}

void basehook::pre_move(premove_fn a, const std::string& hackname)
{
	gethacks<premove_hack>().push_back({a, hackname});
}
void basehook::post_move(postmove_fn a, const std::string& hackname)
{
	gethacks<postmove_hack>().push_back({a, hackname});
}
void basehook::post_paint(paint_fn a, const std::string& hackname)
{
	gethacks<paint_hack>().push_back({a, hackname});
}

vthook* createmove_hook;
vthook* paint_hook;

void hooked_createmove(int seq_num, float frametime, bool active)
{
	static int first;
	if (first++ == 0)
		dcon(NAME "Hooked CreateMove");

	if (ifs::engine->IsInGame())
		runhacks<premove_hack, int, float, bool>("pre-CreateMove", seq_num, frametime, active);

	createmove_hook->unhook();
	ifs::client->CreateMove(seq_num, frametime, active);
	createmove_hook->rehook();

	// Grab the command
	CUserCmd* cmd = ifs::input->GetUserCmd(seq_num);
	CInput::CVerifiedUserCmd& verified =
			((CInput*)ifs::input)->m_pVerifiedCommands[seq_num % 90];

	if (ifs::engine->IsInGame())
		runhacks<postmove_hack, CUserCmd*>("post-CreateMove", cmd);

	// Verify the command
	verified.m_cmd = *cmd;
	verified.m_crc = cmd->GetChecksum();
}

void hooked_paint(VPANEL vpanel, bool repaint, bool force)
{
	paint_hook->unhook();
	ifs::panel->PaintTraverse(vpanel, repaint, force);
	paint_hook->rehook();

	static bool first = true;
	static bool render = false;

	if (first)
			dcon(NAME "Hooked PaintTraverse");
	first = false;

	const char* pname = ifs::panel->GetName(vpanel);

	if (!render && !strcmp(pname, "staticClientDLLPanel"))
		render = true;

	if (!strcmp(pname, "GameMenu"))
		render = false;

	if (render && !strcmp(pname, "MatSystemTopPanel") &&
			ifs::engine->IsPaused() &&
			!ifs::engine->IsTakingScreenshot())
		runhacks<paint_hack>("PaintTraverse");
}

void basehook::init()
{
	createmove_hook = new vthook(ifs::client, 200, 22, hooked_createmove);
	createmove_hook = new vthook(ifs::panel, 200, 41, hooked_paint);
}
void basehook::unload()
{
	delete createmove_hook;
	delete paint_hook;
	gethacks<premove_hack>().clear();
	gethacks<postmove_hack>().clear();
	gethacks<paint_hack>().clear();
}

} /* namespace skim */
