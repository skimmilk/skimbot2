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

extern "C" void libmain()
{
	if (!skim::ifs::load())
		skim::window("Could not load");

	skim::cvar_hook::init();

	skim::con(NAME "Loaded");
	return;
}
