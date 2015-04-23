/*
 * achievement.cpp
 *
 *  Created on: Apr 18, 2015
 *      Author: skim
 */

#include "achievement.h"

#include <vector>
#include <stdlib.h>

#include "const.h"
#include "ifs.h"
#include "debug.h"
#include "exit.h"
#include "sourceutil.h"
#include "sdk/steam.h"
#include "sdk/cvar.h"

namespace skim
{

ConCommand* c_unlock;
ConCommand* c_list;
ConCommand* c_relock;

// TODO: Get the maximum number of achievements runtime
#define NUMACHIEVE 500

// Unlock a random achievement
void ach_unlock()
{
	std::vector<const char*> anchovies;

	for (int i = 0; i < NUMACHIEVE; i++)
	{
		const char* achname = ifs::steam_stats->GetAchievementName(i);
		if (!achname)
			break;

		bool unlocked;
		if (ifs::steam_stats->GetAchievement(achname, &unlocked) && !unlocked)
			anchovies.push_back(achname);
	}

	if (!anchovies.size())
	{
		econ("No free achievements found");
		return;
	}

	const char* picked = anchovies[rand() % anchovies.size()];
	con("Unlocked " + std::string(picked));

	ifs::steam_stats->SetAchievement(picked);
	ifs::steam_stats->StoreStats();
}
void ach_list()
{
	for (int i = 0; i < NUMACHIEVE; i++)
	{
		const char* achname = ifs::steam_stats->GetAchievementName(i);
		if (!achname)
			break;

		con(achname);
	}
}
void ach_relock()
{
	for (int i = 0; i < NUMACHIEVE; i++)
	{
		const char* achname = ifs::steam_stats->GetAchievementName(i);
		if (!achname)
			break;

		ifs::steam_stats->ClearAchievement(achname);
	}

	ifs::steam_stats->StoreStats();
}

void ach_done()
{
	delete c_unlock;
	delete c_list;
	delete c_relock;
}
void achievement::init()
{
	c_unlock = new ConCommand(PREFIX "ach_unlock", ach_unlock, "Unlock a random achievement");
	c_list = new ConCommand(PREFIX "ach_list", ach_list, "List achievement names");
	c_relock = new ConCommand(PREFIX "ach_relock", ach_relock, "Lock achievements");

	srand(time(0));

	exit::handle(ach_done);
}

} /* namespace skim */
