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
#include "sdk/achievable.h"
#include "sdk/engine.h"

namespace skim
{

static ConCommand* unlock;
static ConCommand* list;
static ConCommand* relock;

// TODO: Get the maximum number of achievements runtime
#define NUMACHIEVE 500

// Unlock a random achievement
static void ach_unlock()
{
	auto ach = ifs::engine->GetAchievementMgr();
	int max = ach->GetAchievementCount();
	std::vector<IAchievement*> anchovies;

	for (int i = 0; i < max; i++)
	{
		auto it = ach->GetAchievementByIndex(i);
		if (!it->IsAchieved())
			anchovies.push_back(it);
	}

	if (!anchovies.size())
	{
		econ("No free achievements found");
		return;
	}

	ach->AwardAchievement(
			anchovies[rand() % anchovies.size()]->GetAchievementID());
}
static void ach_list()
{
	for (int i = 0; i < NUMACHIEVE; i++)
	{
		const char* achname = ifs::steam_stats->GetAchievementName(i);
		if (!achname)
			break;

		con(achname);
	}
}
static void ach_relock()
{
	for (int i = 0; i < NUMACHIEVE; i++)
	{
		const char* achname = ifs::steam_stats->GetAchievementName(i);
		if (!achname)
			break;

		ifs::steam_stats->ClearAchievement(achname);
	}

	ifs::steam_stats->StoreStats();
	ifs::engine->GetAchievementMgr()->DownloadUserData();
}

static void ach_done()
{
	delete unlock;
	delete list;
	delete relock;
}
void achievement::init()
{
	unlock = new ConCommand(PREFIX "ach_unlock", ach_unlock, "Unlock a random achievement");
	list = new ConCommand(PREFIX "ach_list", ach_list, "List achievement names");
	relock = new ConCommand(PREFIX "ach_relock", ach_relock, "Lock achievements");

	srand(time(0));

	exit::handle(ach_done);
}

} /* namespace skim */
