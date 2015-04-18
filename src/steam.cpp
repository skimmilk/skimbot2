/*
 * steam.cpp
 *
 *  Created on: Apr 18, 2015
 *      Author: skim
 */

#include "steam.h"

#include "ifs.h"
#include "sdk/steam.h"
#include "sdk/engine.h"

namespace skim
{

bool steam::is_friend(int entnum)
{
	player_info_t inf0;
	ifs::engine->GetPlayerInfo(entnum, &inf0);

	if (!inf0.friendsID || inf0.fakeplayer)
		return false;

	// k_EAccountTypeIndividual = 1;
	// k_EUniversePublic = 1
	CSteamID id;
	id.m_steamid.m_comp.m_EAccountType = 1;
	id.m_steamid.m_comp.m_EUniverse = 1;
	id.m_steamid.m_comp.m_unAccountID = inf0.friendsID;
	id.m_steamid.m_comp.m_unAccountInstance = 1;

	// k_EFriendFlagImmediate = 4
	return ifs::steam_friends->HasFriend(id, 4);
}

} /* namespace skim */
