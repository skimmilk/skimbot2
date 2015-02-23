/*
 * sourceutil.cpp
 *
 *  Created on: Feb 22, 2015
 *      Author: skim
 */

#include "sourceutil.h"

#include "ifs.h"
#include "sdk/cvar.h"

namespace skim
{
// Write the string to the developer console
void con(const std::string& a)
{
	ifs::cvar->ConsolePrintf(a.c_str());
	ifs::cvar->ConsolePrintf("\n");
}

// Write the string to the developer console if developer mode on
void dcon(const std::string& a)
{
	ifs::cvar->ConsoleDPrintf(a.c_str());
	ifs::cvar->ConsolePrintf("\n");
}

// Write the string to the developer console, error
void econ(const std::string& a)
{
	ccon((color){220,10,10, 0}, a);
	ifs::cvar->ConsolePrintf("\n");
}

// Write the string to the developer console as color c
void ccon(const color& c, const std::string& a)
{
	ifs::cvar->ConsoleColorPrintf(c, a.c_str());
	ifs::cvar->ConsolePrintf("\n");
}

} /* namespace skim */
