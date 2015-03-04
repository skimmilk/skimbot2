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
#include "exit.h"
#include "trigger.h"
#include "bhop.h"
#include "spin.h"
#include "sdk/engine.h"

ConCommand* dumpall;
ConCommand* dumpname;
ConCommand* dumpclass;
ConCommand* tfinfo;

extern "C" void libmain()
{
	if (!skim::ifs::load())
		skim::window("Could not load");

	skim::exit::init();

	skim::cvar_hook::init();
	skim::exit::handle(skim::cvar_hook::unload);

	skim::basehook::init();
	skim::exit::handle(skim::basehook::unload);

	skim::trigger::init();
	skim::exit::handle(skim::trigger::unload);

	skim::bhop::init();
	skim::exit::handle(skim::bhop::unload);

	skim::spin::init();
	skim::exit::handle(skim::spin::unload);

	// Test netvars
	dumpall = new ConCommand("dumpall", skim::netvar::dumpnets);
	dumpname = new ConCommand("dumpname", skim::netvar::dumpclasses);
	dumpclass = new ConCommand("dumpclass", [](const CCommand& arg){
		skim::netvar::dumpnets(arg.Arg(1));
	});

	tfinfo = new ConCommand("tfinfo", skim::tfdebug::localinfo);

	skim::exit::handle([](){
		delete dumpall;
		delete dumpname;
		delete dumpclass;
		delete tfinfo;
	});

	skim::con(NAME "Loaded");
	skim::con(NAME "Build " __TIMESTAMP__);
	skim::ifs::engine->ClientCmd_Unrestricted("showconsole");

	return;
}
