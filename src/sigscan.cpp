/*
 * sigscan.cpp
 *
 *  Created on: Feb 22, 2015
 *      Author: skim
 */

#include "sigscan.h"

#include <vector>
#include <fstream>
#include <unistd.h>

namespace skim
{

struct exe_region { const unsigned char* begin; long length; };

// Returns the location of the executable region of libname in the attached application
// Returns 0 on error
static exe_region baseof(pid_t pid, const std::string& libname)
{
	// /proc/$PID/maps file contains the locations of all loaded executables
	std::string proc_str ("/proc/");
	proc_str += std::to_string(pid);
	proc_str += "/maps";

	// Open the maps file and check if opened successfully
	std::ifstream proc_file (proc_str);
	if (proc_file.fail())
		return {0,0};

	std::string line;
	exe_region result;
	// each line in maps file is laid out this way
	// <base ptr>-<end ptr> <permissions> <ignore> <ignore> <ignore> lib.so
	while (std::getline(proc_file, line))
	{
		// Does this line describe where libname is?
		auto position = line.find(libname);
		if (position == line.npos)
			continue;

		// Is this mapping executable?
		position = line.find("r-xp");
		if (position == line.npos)
			continue;

		// Get the base pointer
		std::string map_begin = line.substr(0, line.find('-'));
		result.begin = (const unsigned char*)std::stoul(map_begin, nullptr, 16);

		// End pointer
		std::string map_end = line.substr(line.find('-') + 1, line.find(' '));
		result.length = std::stoul(map_end, nullptr, 16) - (long)result.begin;
	}
	return result;
}

struct sigpoint { unsigned char c; bool is_masked; };
static std::vector<sigpoint> convert_signature(const std::string& pattern)
{
	std::vector<sigpoint> result;
	const std::string lookup = "0123456789ABCDEF";

	bool full = false;
	unsigned char current = 0;

	for (char a : pattern)
	{
		if (a == 'x' || a == 'X' || a == '!')
		{
			if (full)
				result.push_back((sigpoint){0, true});
			full = !full;
			continue;
		}
		else if (a == ' ')
			continue;

		unsigned char nibble = lookup.find(a);
		if (full)
			result.push_back((sigpoint){(unsigned char)(current | nibble), false});
		else
			current = nibble << 4;
		full = !full;
	}
	return result;
}

// hmod is not a module handle on linux, but a ptr to a function inside the module. It is later changed to the module handle.
void* SigScan(const std::string& lib, const char* pat, unsigned int len )
{
	exe_region region = baseof(getpid(), lib);

	// Find module end
	const unsigned char* end = region.begin + region.length - len;

	// Scan for signature
	for ( const unsigned char* p = region.begin; p<=end; ++p )
	{
		unsigned int i = 0;
		do
		{
			if ( (p[i]!=pat[i] ))
					goto cont;
		}
		while ( ++i<len );
		return (void*)p;
		cont:;
	}

	return NULL;
}
void* sigscan(const std::string& libname, const std::string& pattern)
{
	exe_region region = baseof(getpid(), libname);

	if (!region.begin)
		return 0;

	auto sigmask = convert_signature(pattern);

	// Scan for signature
	for ( const unsigned char* p = region.begin; p<=region.begin + region.length; ++p )
	{
		unsigned int i = 0;
		do
		{
			if (sigmask[i].is_masked)
				continue;
			if (p[i] != sigmask[i].c)
				goto cont;
		}
		while ( ++i<sigmask.size());
		return (void*)p;
		cont:;
	}

	return 0;
}

} /* namespace skim */
