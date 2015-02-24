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

extern "C" void libmain()
{
	if (!skim::ifs::load())
		skim::window("Could not load");

	skim::cvar_hook::init();

	skim::con(NAME "Loaded");

	skim::window("Loaded");

	// Test netvars
	new ConCommand("zimbabwe", [](const CCommand& a){
		skim::con(std::to_string(skim::netvar::netoffset(a.Arg(1), a.Arg(2))));
	});
	return;
}
