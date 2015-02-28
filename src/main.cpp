/*
 * main.cpp
 *
 *  Created on: Feb 19, 2015
 *      Author: skim
 */

#include "ifs.h"
#include "cvarhook.h"
#include "debug.h"
#include "const.h"
#include "sourceutil.h"
#include "netvar.h"
#include "basehook.h"
#include "tfdebug.h"
#include "sdk/engine.h"

extern "C" void libmain()
{
	if (!skim::ifs::load())
		skim::window("Could not load");

	skim::cvar_hook::init();
	skim::basehook::init();

	skim::con(NAME "Loaded");
	skim::con(NAME "Build " __TIMESTAMP__);
	skim::ifs::engine->ClientCmd_Unrestricted("showconsole");

	// Test netvars
	new ConCommand("dumpall", skim::netvar::dumpnets);
	new ConCommand("dumpname", skim::netvar::dumpclasses);
	new ConCommand("dumpclass", [](const CCommand& arg){
		skim::netvar::dumpnets(arg.Arg(1));
	});

	new ConCommand("tfinfo", skim::tfdebug::localinfo);

	return;
}
