/*
 * steam.h
 *
 *  Created on: Feb 22, 2015
 *      Author: skim
 */

#ifndef STEAM_H_
#define STEAM_H_

#include <sys/types.h>

#define STEAMWORKS_STRUCT_RETURN_0(returnType, functionName) virtual returnType functionName() = 0;
#define STEAMWORKS_STRUCT_RETURN_1(returnType, functionName, arg1Type, arg1Name) virtual returnType functionName( arg1Type arg1Name ) = 0;
#define STEAMWORKS_STRUCT_RETURN_2(returnType, functionName, arg1Type, arg1Name, arg2Type, arg2Name) virtual returnType functionName( arg1Type arg1Name, arg2Type arg2Name ) = 0;
#define STEAMWORKS_STRUCT_RETURN_3(returnType, functionName, arg1Type, arg1Name, arg2Type, arg2Name, arg3Type, arg3Name) virtual returnType functionName( arg1Type arg1Name, arg2Type arg2Name, arg3Type arg3Name ) = 0;

typedef u_int64_t SteamAPICall_t;
typedef int HSteamPipe;
typedef int HSteamUser;
typedef int AppId_t;
typedef int int32;

typedef int EUniverse;
typedef int EChatEntryType;
typedef int EAccountType;
typedef int EFriendRelationship;
typedef int ELeaderboardDataRequest;
typedef int ELeaderboardDisplayType;
typedef int ELeaderboardSortMethod;
typedef int ELeaderboardUploadScoreMethod;
typedef int EOverlayToStoreFlag;
typedef int EPersonaState;
typedef int FriendGameInfo_t;
typedef int EUserRestriction;
typedef int LeaderboardEntry_t;
typedef int SteamLeaderboard_t;
typedef int SteamAPIWarningMessageHook_t;
typedef int SteamLeaderboardEntries_t;
typedef int UGCHandle_t;

typedef void* ISteamApps;
typedef void* ISteamController;
typedef void* ISteamGameServer;
typedef void* ISteamGameServerStats;
typedef void* ISteamHTTP;
typedef void* ISteamMatchmaking;
typedef void* ISteamMatchmakingServers;
typedef void* ISteamNetworking;
typedef void* ISteamRemoteStorage;
typedef void* ISteamScreenshots;
typedef void* ISteamUnifiedMessages;
typedef void* ISteamUser;
typedef void* ISteamUtils;

class CSteamID
{
public:
	// These are defined here to prevent accidental implicit conversion of a u32AccountID to a CSteamID.
	// If you get a compiler error about an ambiguous constructor/function then it may be because you're
	// passing a 32-bit int to a function that takes a CSteamID. You should explicitly create the SteamID
	// using the correct Universe and account Type/Instance values.
	// 64 bits total
	union SteamID_t
	{
		struct SteamIDComponent_t
		{
			uint32 m_unAccountID : 32; // unique account identifier
			unsigned int	m_unAccountInstance : 20; // dynamic instance ID
			unsigned int	m_EAccountType : 4; // type of account - can't show as EAccountType, due to signed / unsigned difference
			EUniverse m_EUniverse : 8; // universe this account belongs to
		} m_comp;
		u_int64_t m_unAll64Bits;
	} m_steamid;
};

class CGameID
{
public:
	enum EGameIDType
	{
		k_EGameIDTypeApp = 0,
		k_EGameIDTypeGameMod = 1,
		k_EGameIDTypeShortcut = 2,
		k_EGameIDTypeP2P = 3,
	};
	struct GameID_t
	{
		unsigned int m_nAppID : 24;
		unsigned int m_nType : 8;
		unsigned int m_nModID : 32;
	};
	union
	{
		u_int64_t m_ulGameID;
		GameID_t m_gameID;
	};
};

// Version 014
class ISteamFriends
{
public:
	// returns the local players name - guaranteed to not be NULL.
	// this is the same name as on the users community profile page
	// this is stored in UTF-8 format
	// like all the other interface functions that return a char *, it's important that this pointer is not saved
	// off; it will eventually be free'd or re-allocated
	virtual const char *GetPersonaName() = 0;
	// sets the player name, stores it on the server and publishes the changes to all friends who are online
	virtual SteamAPICall_t SetPersonaName( const char *pchPersonaName ) = 0;
	// gets the status of the current user
	virtual EPersonaState GetPersonaState() = 0;
	// friend iteration
	// takes a set of k_EFriendFlags, and returns the number of users the client knows about who meet that criteria
	// then GetFriendByIndex() can then be used to return the id's of each of those users
	virtual int GetFriendCount( int iFriendFlags ) = 0;
	// returns the steamID of a user
	// iFriend is a index of range [0, GetFriendCount())
	// iFriendsFlags must be the same value as used in GetFriendCount()
	// the returned CSteamID can then be used by all the functions below to access details about the user
	STEAMWORKS_STRUCT_RETURN_2(CSteamID, GetFriendByIndex, int, iFriend, int, iFriendFlags) /*virtual CSteamID GetFriendByIndex( int iFriend, int iFriendFlags ) = 0;*/
	// returns a relationship to a user
	virtual EFriendRelationship GetFriendRelationship( CSteamID steamIDFriend ) = 0;
	// returns the current status of the specified user
	// this will only be known by the local user if steamIDFriend is in their friends list; on the same game server; in a chat room or lobby; or in a small group with the local user
	virtual EPersonaState GetFriendPersonaState( CSteamID steamIDFriend ) = 0;
	// returns the name another user - guaranteed to not be NULL.
	// same rules as GetFriendPersonaState() apply as to whether or not the user knowns the name of the other user
	// note that on first joining a lobby, chat room or game server the local user will not known the name of the other users automatically; that information will arrive asyncronously
	//
	virtual const char *GetFriendPersonaName( CSteamID steamIDFriend ) = 0;
	// returns true if the friend is actually in a game, and fills in pFriendGameInfo with an extra details
	virtual bool GetFriendGamePlayed( CSteamID steamIDFriend, FriendGameInfo_t *pFriendGameInfo ) = 0;
	// accesses old friends names - returns an empty string when their are no more items in the history
	virtual const char *GetFriendPersonaNameHistory( CSteamID steamIDFriend, int iPersonaName ) = 0;
	virtual const char *GetPlayerNickname( CSteamID steamIDPlayer ) = 0;
	// returns true if the specified user meets any of the criteria specified in iFriendFlags
	// iFriendFlags can be the union (binary or, |) of one or more k_EFriendFlags values
	virtual bool HasFriend( CSteamID steamIDFriend, int iFriendFlags ) = 0;
	// clan (group) iteration and access functions
	virtual int GetClanCount() = 0;
	STEAMWORKS_STRUCT_RETURN_1(CSteamID, GetClanByIndex, int, iClan) /*virtual CSteamID GetClanByIndex( int iClan ) = 0;*/
	virtual const char *GetClanName( CSteamID steamIDClan ) = 0;
	virtual const char *GetClanTag( CSteamID steamIDClan ) = 0;
	virtual bool GetClanActivityCounts( CSteamID steamID, int *pnOnline, int *pnInGame, int *pnChatting ) = 0;
	virtual SteamAPICall_t DownloadClanActivityCounts( CSteamID groupIDs[], int nIds ) = 0;
	// iterators for getting users in a chat room, lobby, game server or clan
	// note that large clans that cannot be iterated by the local user
	// steamIDSource can be the steamID of a group, game server, lobby or chat room
	virtual int GetFriendCountFromSource( CSteamID steamIDSource ) = 0;
	STEAMWORKS_STRUCT_RETURN_2(CSteamID, GetFriendFromSourceByIndex, CSteamID, steamIDSource, int, iFriend) /*virtual CSteamID GetFriendFromSourceByIndex( CSteamID steamIDSource, int iFriend ) = 0;*/
	// returns true if the local user can see that steamIDUser is a member or in steamIDSource
	virtual bool IsUserInSource( CSteamID steamIDUser, CSteamID steamIDSource ) = 0;
	// User is in a game pressing the talk button (will suppress the microphone for all voice comms from the Steam friends UI)
	virtual void SetInGameVoiceSpeaking( CSteamID steamIDUser, bool bSpeaking ) = 0;
	// activates the game overlay, with an optional dialog to open
	// valid options are "Friends", "Community", "Players", "Settings", "LobbyInvite", "OfficialGameGroup", "Stats", "Achievements"
	virtual void ActivateGameOverlay( const char *pchDialog ) = 0;
	// activates game overlay to a specific place
	// valid options are
	//		"steamid" - opens the overlay web browser to the specified user or groups profile
	//		"chat" - opens a chat window to the specified user, or joins the group chat
	//		"stats" - opens the overlay web browser to the specified user's stats
	//		"achievements" - opens the overlay web browser to the specified user's achievements
	virtual void ActivateGameOverlayToUser( const char *pchDialog, CSteamID steamID ) = 0;
	// activates game overlay web browser directly to the specified URL
	// full address with protocol type is required, e.g. http://www.steamgames.com/
	virtual void ActivateGameOverlayToWebPage( const char *pchURL ) = 0;
	// activates game overlay to store page for app
	virtual void ActivateGameOverlayToStore( AppId_t nAppID, EOverlayToStoreFlag eFlag ) = 0;
	// Mark a target user as 'played with'. This is a client-side only feature that requires that the calling user is
	// in game
	virtual void SetPlayedWith( CSteamID steamIDUserPlayedWith ) = 0;
	// activates game overlay to open the invite dialog. Invitations will be sent for the provided lobby.
	// You can also use ActivateGameOverlay( "LobbyInvite" ) to allow the user to create invitations for their current public lobby.
	virtual void ActivateGameOverlayInviteDialog( CSteamID steamIDLobby ) = 0;
	// gets the small (32x32) avatar of the current user, which is a handle to be used in IClientUtils::GetImageRGBA(), or 0 if none set
	virtual int GetSmallFriendAvatar( CSteamID steamIDFriend ) = 0;
	// gets the medium (64x64) avatar of the current user, which is a handle to be used in IClientUtils::GetImageRGBA(), or 0 if none set
	virtual int GetMediumFriendAvatar( CSteamID steamIDFriend ) = 0;
	// gets the large (184x184) avatar of the current user, which is a handle to be used in IClientUtils::GetImageRGBA(), or 0 if none set
	// returns -1 if this image has yet to be loaded, in this case wait for a AvatarImageLoaded_t callback and then call this again
	virtual int GetLargeFriendAvatar( CSteamID steamIDFriend ) = 0;
	// requests information about a user - persona name & avatar
	// if bRequireNameOnly is set, then the avatar of a user isn't downloaded
	// - it's a lot slower to download avatars and churns the local cache, so if you don't need avatars, don't request them
	// if returns true, it means that data is being requested, and a PersonaStateChanged_t callback will be posted when it's retrieved
	// if returns false, it means that we already have all the details about that user, and functions can be called immediately
	virtual bool RequestUserInformation( CSteamID steamIDUser, bool bRequireNameOnly ) = 0;
	// requests information about a clan officer list
	// when complete, data is returned in ClanOfficerListResponse_t call result
	// this makes available the calls below
	// you can only ask about clans that a user is a member of
	// note that this won't download avatars automatically; if you get an officer,
	// and no avatar image is available, call RequestUserInformation( steamID, false ) to download the avatar
	virtual SteamAPICall_t RequestClanOfficerList( CSteamID steamIDClan ) = 0;
	// iteration of clan officers - can only be done when a RequestClanOfficerList() call has completed
	// returns the steamID of the clan owner
	STEAMWORKS_STRUCT_RETURN_1(CSteamID, GetClanOwner, CSteamID, steamIDClan) /*virtual CSteamID GetClanOwner( CSteamID steamIDClan ) = 0;*/
	// returns the number of officers in a clan (including the owner)
	virtual int GetClanOfficerCount( CSteamID steamIDClan ) = 0;
	// returns the steamID of a clan officer, by index, of range [0,GetClanOfficerCount)
	STEAMWORKS_STRUCT_RETURN_2(CSteamID, GetClanOfficerByIndex, CSteamID, steamIDClan, int, iOfficer) /*virtual CSteamID GetClanOfficerByIndex( CSteamID steamIDClan, int iOfficer ) = 0;*/
	// if current user is chat restricted, he can't send or receive any text/voice chat messages.
	// the user can't see custom avatars. But the user can be online and send/recv game invites.
	// a chat restricted user can't add friends or join any groups.
	virtual EUserRestriction GetUserRestrictions() = 0;
	// Rich Presence data is automatically shared between friends who are in the same game
	// Each user has a set of Key/Value pairs
	// Up to 20 different keys can be set
	// There are two magic keys:
	//		"status"  - a UTF-8 string that will show up in the 'view game info' dialog in the Steam friends list
	//		"connect" - a UTF-8 string that contains the command-line for how a friend can connect to a game
	// GetFriendRichPresence() returns an empty string "" if no value is set
	// SetRichPresence() to a NULL or an empty string deletes the key
	// You can iterate the current set of keys for a friend with GetFriendRichPresenceKeyCount()
	// and GetFriendRichPresenceKeyByIndex() (typically only used for debugging)
	virtual bool SetRichPresence( const char *pchKey, const char *pchValue ) = 0;
	virtual void ClearRichPresence() = 0;
	virtual const char *GetFriendRichPresence( CSteamID steamIDFriend, const char *pchKey ) = 0;
	virtual int GetFriendRichPresenceKeyCount( CSteamID steamIDFriend ) = 0;
	virtual const char *GetFriendRichPresenceKeyByIndex( CSteamID steamIDFriend, int iKey ) = 0;
	virtual void RequestFriendRichPresence( CSteamID steamIDFriend ) = 0;
	// rich invite support
	// if the target accepts the invite, the pchConnectString gets added to the command-line for launching the game
	// if the game is already running, a GameRichPresenceJoinRequested_t callback is posted containing the connect string
	// invites can only be sent to friends
	virtual bool InviteUserToGame( CSteamID steamIDFriend, const char *pchConnectString ) = 0;
	// recently-played-with friends iteration
	// this iterates the entire list of users recently played with, across games
	// GetFriendCoplayTime() returns as a unix time
	virtual int GetCoplayFriendCount() = 0;
	STEAMWORKS_STRUCT_RETURN_1(CSteamID, GetCoplayFriend, int, iCoplayFriend) /*virtual CSteamID GetCoplayFriend( int iCoplayFriend ) = 0;*/
	virtual int GetFriendCoplayTime( CSteamID steamIDFriend ) = 0;
	virtual AppId_t GetFriendCoplayGame( CSteamID steamIDFriend ) = 0;
	// chat interface for games
	// this allows in-game access to group (clan) chats from in the game
	// the behavior is somewhat sophisticated, because the user may or may not be already in the group chat from outside the game or in the overlay
	// use ActivateGameOverlayToUser( "chat", steamIDClan ) to open the in-game overlay version of the chat
	virtual SteamAPICall_t JoinClanChatRoom( CSteamID steamIDClan ) = 0;
	virtual bool LeaveClanChatRoom( CSteamID steamIDClan ) = 0;
	virtual int GetClanChatMemberCount( CSteamID steamIDClan ) = 0;
	STEAMWORKS_STRUCT_RETURN_2(CSteamID, GetChatMemberByIndex, CSteamID, steamIDClan, int, iUser) /*virtual CSteamID GetChatMemberByIndex( CSteamID steamIDClan, int iUser ) = 0;*/
	virtual bool SendClanChatMessage( CSteamID steamIDClanChat, const char *pchText ) = 0;
	virtual int GetClanChatMessage( CSteamID steamIDClanChat, int iMessage, void *prgchText, int cchTextMax, EChatEntryType *peChatEntryType, CSteamID *pSteamIDChatter ) = 0;
	virtual bool IsClanChatAdmin( CSteamID steamIDClanChat, CSteamID steamIDUser ) = 0;
	// interact with the Steam (game overlay / desktop)
	virtual bool IsClanChatWindowOpenInSteam( CSteamID steamIDClanChat ) = 0;
	virtual bool OpenClanChatWindowInSteam( CSteamID steamIDClanChat ) = 0;
	virtual bool CloseClanChatWindowInSteam( CSteamID steamIDClanChat ) = 0;
	// peer-to-peer chat interception
	// this is so you can show P2P chats inline in the game
	virtual bool SetListenForFriendsMessages( bool bInterceptEnabled ) = 0;
	virtual bool ReplyToFriendMessage( CSteamID steamIDFriend, const char *pchMsgToSend ) = 0;
	virtual int GetFriendMessage( CSteamID steamIDFriend, int iMessageID, void *pvData, int cubData, EChatEntryType *peChatEntryType ) = 0;
	// following apis
	virtual SteamAPICall_t GetFollowerCount( CSteamID steamID ) = 0;
	virtual SteamAPICall_t IsFollowing( CSteamID steamID ) = 0;
	virtual SteamAPICall_t EnumerateFollowingList( uint32 unStartIndex ) = 0;
};

// version 011
class ISteamUserStats
{
public:
	// Ask the server to send down this user's data and achievements for this game
	virtual bool RequestCurrentStats() = 0;
	// Data accessors
	virtual bool GetStat( const char *pchName, int32 *pData ) = 0;
	virtual bool GetStat( const char *pchName, float *pData ) = 0;
	// Set / update data
	virtual bool SetStat( const char *pchName, int32 nData ) = 0;
	virtual bool SetStat( const char *pchName, float fData ) = 0;
	virtual bool UpdateAvgRateStat( const char *pchName, float flCountThisSession, double dSessionLength ) = 0;
	// Achievement flag accessors
	virtual bool GetAchievement( const char *pchName, bool *pbAchieved ) = 0;
	virtual bool SetAchievement( const char *pchName ) = 0;
	virtual bool ClearAchievement( const char *pchName ) = 0;
	// Get the achievement status, and the time it was unlocked if unlocked.
	// If the return value is true, but the unlock time is zero, that means it was unlocked before Steam
	// began tracking achievement unlock times (December 2009). Time is seconds since January 1, 1970.
	virtual bool GetAchievementAndUnlockTime( const char *pchName, bool *pbAchieved, uint32 *punUnlockTime ) = 0;
	// Store the current data on the server, will get a callback when set
	// And one callback for every new achievement
	//
	// If the callback has a result of k_EResultInvalidParam, one or more stats
	// uploaded has been rejected, either because they broke constraints
	// or were out of date. In this case the server sends back updated values.
	// The stats should be re-iterated to keep in sync.
	virtual bool StoreStats() = 0;
	// Achievement / GroupAchievement metadata

	// Gets the icon of the achievement, which is a handle to be used in IClientUtils::GetImageRGBA(), or 0 if none set.
	// A return value of 0 may indicate we are still fetching data, and you can wait for the UserAchievementIconFetched_t callback
	// which will notify you when the bits are ready. If the callback still returns zero, then there is no image set for the
	// specified achievement.
	virtual int GetAchievementIcon( const char *pchName ) = 0;
	// Get general attributes (display name, desc, etc) for an Achievement
	virtual const char *GetAchievementDisplayAttribute( const char *pchName, const char *pchKey ) = 0;
	// Achievement progress - triggers an AchievementProgress callback, that is all.
	// Calling this w/ N out of N progress will NOT set the achievement, the game must still do that.
	virtual bool IndicateAchievementProgress( const char *pchName, uint32 nCurProgress, uint32 nMaxProgress ) = 0;
	// Get the number of achievements
	virtual uint32 GetNumAchievements( CGameID nGameID ) = 0;
	// Get achievement name iAchievement in [0,GetNumAchievements)
	virtual const char *GetAchievementName( uint32 iAchievement ) = 0;
	// Friends stats & achievements

	// downloads stats for the user
	// returns a UserStatsReceived_t received when completed
	// if the other user has no stats, UserStatsReceived_t.m_eResult will be set to k_EResultFail
	// these stats won't be auto-updated; you'll need to call RequestUserStats() again to refresh any data
	virtual SteamAPICall_t RequestUserStats( CSteamID steamIDUser ) = 0;
	// requests stat information for a user, usable after a successful call to RequestUserStats()
	virtual bool GetUserStat( CSteamID steamIDUser, const char *pchName, int32 *pData ) = 0;
	virtual bool GetUserStat( CSteamID steamIDUser, const char *pchName, float *pData ) = 0;
	virtual bool GetUserAchievement( CSteamID steamIDUser, const char *pchName, bool *pbAchieved ) = 0;
	// See notes for GetAchievementAndUnlockTime above
	virtual bool GetUserAchievementAndUnlockTime( CSteamID steamIDUser, const char *pchName, bool *pbAchieved, uint32 *punUnlockTime ) = 0;
	// Reset stats
	virtual bool ResetAllStats( bool bAchievementsToo ) = 0;
	// Leaderboard functions

	// asks the Steam back-end for a leaderboard by name, and will create it if it's not yet
	// This call is asynchronous, with the result returned in LeaderboardFindResult_t
	virtual SteamAPICall_t FindOrCreateLeaderboard( const char *pchLeaderboardName, ELeaderboardSortMethod eLeaderboardSortMethod, ELeaderboardDisplayType eLeaderboardDisplayType ) = 0;
	// as above, but won't create the leaderboard if it's not found
	// This call is asynchronous, with the result returned in LeaderboardFindResult_t
	virtual SteamAPICall_t FindLeaderboard( const char *pchLeaderboardName ) = 0;
	// returns the name of a leaderboard
	virtual const char *GetLeaderboardName( SteamLeaderboard_t hSteamLeaderboard ) = 0;
	// returns the total number of entries in a leaderboard, as of the last request
	virtual int GetLeaderboardEntryCount( SteamLeaderboard_t hSteamLeaderboard ) = 0;
	// returns the sort method of the leaderboard
	virtual ELeaderboardSortMethod GetLeaderboardSortMethod( SteamLeaderboard_t hSteamLeaderboard ) = 0;
	// returns the display type of the leaderboard
	virtual ELeaderboardDisplayType GetLeaderboardDisplayType( SteamLeaderboard_t hSteamLeaderboard ) = 0;
	// Asks the Steam back-end for a set of rows in the leaderboard.
	// This call is asynchronous, with the result returned in LeaderboardScoresDownloaded_t
	// LeaderboardScoresDownloaded_t will contain a handle to pull the results from GetDownloadedLeaderboardEntries() (below)
	// You can ask for more entries than exist, and it will return as many as do exist.
	// k_ELeaderboardDataRequestGlobal requests rows in the leaderboard from the full table, with nRangeStart & nRangeEnd in the range [1, TotalEntries]
	// k_ELeaderboardDataRequestGlobalAroundUser requests rows around the current user, nRangeStart being negate
	//   e.g. DownloadLeaderboardEntries( hLeaderboard, k_ELeaderboardDataRequestGlobalAroundUser, -3, 3 ) will return 7 rows, 3 before the user, 3 after
	// k_ELeaderboardDataRequestFriends requests all the rows for friends of the current user
	virtual SteamAPICall_t DownloadLeaderboardEntries( SteamLeaderboard_t hSteamLeaderboard, ELeaderboardDataRequest eLeaderboardDataRequest, int nRangeStart, int nRangeEnd ) = 0;
	// as above, but downloads leaderboard entries for an arbitrary set of users - ELeaderboardDataRequest is k_ELeaderboardDataRequestUsers
	// if a user doesn't have a leaderboard entry, they won't be included in the result
	// a max of 100 users can be downloaded at a time, with only one outstanding call at a time
	virtual SteamAPICall_t DownloadLeaderboardEntriesForUsers( SteamLeaderboard_t hSteamLeaderboard, CSteamID *prgUsers, int cUsers ) = 0;
	// Returns data about a single leaderboard entry
	// use a for loop from 0 to LeaderboardScoresDownloaded_t::m_cEntryCount to get all the downloaded entries
	// e.g.
	//		void OnLeaderboardScoresDownloaded( LeaderboardScoresDownloaded_t *pLeaderboardScoresDownloaded )
	//		{
	//			for ( int index = 0; index < pLeaderboardScoresDownloaded->m_cEntryCount; index++ )
	//			{
	//				LeaderboardEntry_t leaderboardEntry;
	//				int32 details[3];		// we know this is how many we've stored previously
	//				GetDownloadedLeaderboardEntry( pLeaderboardScoresDownloaded->m_hSteamLeaderboardEntries, index, &leaderboardEntry, details, 3 );
	//				assert( leaderboardEntry.m_cDetails == 3 );
	//				...
	//			}
	// once you've accessed all the entries, the data will be free'd, and the SteamLeaderboardEntries_t handle will become invalid
	virtual bool GetDownloadedLeaderboardEntry( SteamLeaderboardEntries_t hSteamLeaderboardEntries, int index, LeaderboardEntry_t *pLeaderboardEntry, int32 pDetails[], int cDetailsMax ) = 0;
	// Uploads a user score to the Steam back-end.
	// This call is asynchronous, with the result returned in LeaderboardScoreUploaded_t
	// Details are extra game-defined information regarding how the user got that score
	// pScoreDetails points to an array of int32's, cScoreDetailsCount is the number of int32's in the list
	virtual SteamAPICall_t UploadLeaderboardScore( SteamLeaderboard_t hSteamLeaderboard, ELeaderboardUploadScoreMethod eLeaderboardUploadScoreMethod, int32 nScore, const int32 *pScoreDetails, int cScoreDetailsCount ) = 0;
	// Attaches a piece of user generated content the user's entry on a leaderboard.
	// hContent is a handle to a piece of user generated content that was shared using ISteamUserRemoteStorage::FileShare().
	// This call is asynchronous, with the result returned in LeaderboardUGCSet_t.
	virtual SteamAPICall_t AttachLeaderboardUGC( SteamLeaderboard_t hSteamLeaderboard, UGCHandle_t hUGC ) = 0;
	// Retrieves the number of players currently playing your game (online + offline)
	// This call is asynchronous, with the result returned in NumberOfCurrentPlayers_t
	virtual SteamAPICall_t GetNumberOfCurrentPlayers() = 0;
	// Requests that Steam fetch data on the percentage of players who have received each achievement
	// for the game globally.
	// This call is asynchronous, with the result returned in GlobalAchievementPercentagesReady_t.
	virtual SteamAPICall_t RequestGlobalAchievementPercentages() = 0;
	// Get the info on the most achieved achievement for the game, returns an iterator index you can use to fetch
	// the next most achieved afterwards.  Will return -1 if there is no data on achievement
	// percentages (ie, you haven't called RequestGlobalAchievementPercentages and waited on the callback).
	virtual int GetMostAchievedAchievementInfo( char *pchName, uint32 unNameBufLen, float *pflPercent, bool *pbAchieved ) = 0;
	// Get the info on the next most achieved achievement for the game. Call this after GetMostAchievedAchievementInfo or another
	// GetNextMostAchievedAchievementInfo call passing the iterator from the previous call. Returns -1 after the last
	// achievement has been iterated.
	virtual int GetNextMostAchievedAchievementInfo( int iIteratorPrevious, char *pchName, uint32 unNameBufLen, float *pflPercent, bool *pbAchieved ) = 0;
	// Returns the percentage of users who have achieved the specified achievement.
	virtual bool GetAchievementAchievedPercent( const char *pchName, float *pflPercent ) = 0;
	// Requests global stats data, which is available for stats marked as "aggregated".
	// This call is asynchronous, with the results returned in GlobalStatsReceived_t.
	// nHistoryDays specifies how many days of day-by-day history to retrieve in addition
	// to the overall totals. The limit is 60.
	virtual SteamAPICall_t RequestGlobalStats( int nHistoryDays ) = 0;
	// Gets the lifetime totals for an aggregated stat
	virtual bool GetGlobalStat( const char *pchStatName, int64_t *pData ) = 0;
	virtual bool GetGlobalStat( const char *pchStatName, double *pData ) = 0;
	// Gets history for an aggregated stat. pData will be filled with daily values, starting with today.
	// So when called, pData[0] will be today, pData[1] will be yesterday, and pData[2] will be two days ago,
	// etc. cubData is the size in bytes of the pubData buffer. Returns the number of
	// elements actually set.
	virtual int32 GetGlobalStatHistory( const char *pchStatName, int64_t *pData, uint32 cubData ) = 0;
	virtual int32 GetGlobalStatHistory( const char *pchStatName, double *pData, uint32 cubData ) = 0;
};



// Version 012
class ISteamClient
{
public:
	// Creates a communication pipe to the Steam client
	virtual HSteamPipe CreateSteamPipe() = 0;
	// Releases a previously created communications pipe
	virtual bool BReleaseSteamPipe( HSteamPipe hSteamPipe ) = 0;
	// connects to an existing global user, failing if none exists
	// used by the game to coordinate with the steamUI
	virtual HSteamUser ConnectToGlobalUser( HSteamPipe hSteamPipe ) = 0;
	// used by game servers, create a steam user that won't be shared with anyone else
	virtual HSteamUser CreateLocalUser( HSteamPipe *phSteamPipe, EAccountType eAccountType ) = 0;
	// removes an allocated user
	virtual void ReleaseUser( HSteamPipe hSteamPipe, HSteamUser hUser ) = 0;
	// retrieves the ISteamUser interface associated with the handle
	virtual ISteamUser *GetISteamUser( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion ) = 0;
	// retrieves the ISteamGameServer interface associated with the handle
	virtual ISteamGameServer *GetISteamGameServer( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion ) = 0;
	// set the local IP and Port to bind to
	// this must be set before CreateLocalUser()
	virtual void SetLocalIPBinding( uint32 unIP, short usPort ) = 0;
	// returns the ISteamFriends interface
	virtual ISteamFriends *GetISteamFriends( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion ) = 0;
	// returns the ISteamUtils interface
	virtual ISteamUtils *GetISteamUtils( HSteamPipe hSteamPipe, const char *pchVersion ) = 0;
	// returns the ISteamMatchmaking interface
	virtual ISteamMatchmaking *GetISteamMatchmaking( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion ) = 0;
	// returns the ISteamMatchmakingServers interface
	virtual ISteamMatchmakingServers *GetISteamMatchmakingServers( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion ) = 0;
	// returns a generic interface
	virtual void *GetISteamGenericInterface( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion ) = 0;
	// returns the ISteamUserStats interface
	virtual ISteamUserStats *GetISteamUserStats( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion ) = 0;
	// returns game server stats interface
	virtual ISteamGameServerStats *GetISteamGameServerStats( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion ) = 0;
	// returns apps interface
	virtual ISteamApps *GetISteamApps( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion ) = 0;
	// networking
	virtual ISteamNetworking *GetISteamNetworking( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion ) = 0;
	// remote storage
	virtual ISteamRemoteStorage *GetISteamRemoteStorage( HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char *pchVersion ) = 0;
	virtual ISteamScreenshots *GetISteamScreenshots( HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char *pchVersion ) = 0;
	// this needs to be called every frame to process matchmaking results
	// redundant if you're already calling SteamAPI_RunCallbacks()
	virtual void RunFrame() = 0;
	// returns the number of IPC calls made since the last time this function was called
	// Used for perf debugging so you can understand how many IPC calls your game makes per frame
	// Every IPC call is at minimum a thread context switch if not a process one so you want to rate
	// control how often you do them.
	virtual uint32 GetIPCCallCount() = 0;
	// API warning handling
	// 'int' is the severity; 0 for msg, 1 for warning
	// 'const char *' is the text of the message
	// callbacks will occur directly after the API function is called that generated the warning or message
	virtual void SetWarningMessageHook( SteamAPIWarningMessageHook_t pFunction ) = 0;
	// Trigger global shutdown for the DLL
	virtual bool BShutdownIfAllPipesClosed() = 0;
	virtual ISteamHTTP* GetISteamHTTP( HSteamUser hSteamuser, HSteamPipe hSteamPipe, const char *pchVersion ) = 0;
	virtual ISteamUnifiedMessages* GetISteamUnifiedMessages( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion ) = 0;
	virtual ISteamController* GetISteamController( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char *pchVersion ) = 0;
};

#endif /* STEAM_H_ */
