/*
 * exit.cpp
 *
 *  Created on: Feb 28, 2015
 *      Author: skim
 */

#include "exit.h"

#include <vector>
#include <algorithm>
#include "const.h"
#include "sdk/cvar.h"

namespace skim
{

static ConCommand* exitcmd;
static std::vector<exit::atexit_fn> handlers;

void exit::init()
{
	exitcmd = new ConCommand(PREFIX "exit", exit::unload);
	exit::handle([](){delete exitcmd;});
}

void exit::unload()
{
	for (const auto& a : handlers)
		a();
	handlers.clear();
}
void exit::handle(exit::atexit_fn a)
{
	handlers.push_back(a);
}

} /* namespace skim */
