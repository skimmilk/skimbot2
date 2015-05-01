/*
 * tfcache.cpp
 *
 *  Created on: Apr 30, 2015
 *      Author: skim
 */

#include "tfcache.h"

#include <algorithm>

#include "ifs.h"
#include "basehook.h"
#include "exit.h"
#include "const.h"
#include "sdk/cliententity.h"

namespace skim
{

struct networked_position
{
	Vector position;
	bool valid;
};

networked_position* pos_prev;
networked_position* pos_current;

bool tfcache::is_moving(int entnum)
{
	if (entnum < 1 || entnum >= ENT_MAX)
		return false;

	networked_position& prev = pos_prev[entnum];
	networked_position& current = pos_current[entnum];

	if (!prev.valid || !current.valid)
		return false;
	return	prev.position.x != current.position.x ||
			prev.position.y != current.position.y ||
			prev.position.z != current.position.z;
}

static void frame(CUserCmd*)
{
	std::swap(pos_prev, pos_current);
	for (int i = ENT_START; i < ENT_MAX; i++)
	{
		auto ent = ifs::entities->GetClientEntity(i);
		if (!ent)
			pos_current[i].valid = false;
		else
		{
			pos_current[i].position = ent->GetAbsOrigin();
			pos_current[i].valid = !ent->IsDormant();
		}
	}
}

static void unload()
{
	delete[] pos_prev;
	delete[] pos_current;
}
void tfcache::init()
{
	pos_prev = new networked_position[2048]();
	pos_current = new networked_position[2048]();

	exit::handle(unload);
	basehook::post_move(frame, "tfcache");
}

} /* namespace skim */
