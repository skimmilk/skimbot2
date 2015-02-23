/*
 * debug.cpp
 *
 *  Created on: Feb 22, 2015
 *      Author: skim
 */

#include "debug.h"

#include <string>
#include <stdlib.h>

namespace skim
{

int window(const std::string& q)
{
	std::string s = "zenity --question --text='";
	s += q;
	s += "'";
	int proc = system(s.c_str());
	return !WEXITSTATUS(proc);
}

} /* namespace skim */
