/*
 * common.h
 *
 *  Created on: Feb 18, 2015
 *      Author: skim
 *
 *  Use: Contains commonly used structs
 */

#ifndef COMMON_H_
#define COMMON_H_

typedef void* (*CreateInterfaceFn)(const char* name, int* code);

struct color
{
	unsigned char red, blue, green, alpha;
};

struct Vector
{
	float x, y, z;
};

struct Vector2D
{
	float x, y;
	Vector2D(float _x, float _y) : x(_x), y(_y) {}
	Vector2D() : x(0), y(0) {}
};

struct QAngle
{
	float x, y, z;
};

#define MAX_PLAYER_NAME_LENGTH 32
#define MAX_CUSTOM_FILES 4
#define SIGNED_GUID_LEN 32
typedef unsigned int CRC32_t;

typedef struct player_info_s
{
	// scoreboard information
	char			name[MAX_PLAYER_NAME_LENGTH];
	// local server user ID, unique while server is running
	int				userID;
	// global unique player identifer
	char			guid[SIGNED_GUID_LEN + 1];
	// friends identification number
	unsigned int	friendsID;
	// friends name
	char			friendsName[MAX_PLAYER_NAME_LENGTH];
	// true, if player is a bot controlled by game.dll
	bool			fakeplayer;
	// true if player is the HLTV proxy
	bool			ishltv;
#if defined( REPLAY_ENABLED )
	// true if player is the Replay proxy
	bool			isreplay;
#endif
	// custom files CRC for this player
	CRC32_t			customFiles[MAX_CUSTOM_FILES];
	// this counter increases each time the server downloaded a new file
	unsigned char	filesDownloaded;
} player_info_t;

typedef unsigned int uint32;
typedef unsigned int uint;
struct model_t;
class SurfInfo;
class CAudioSource;
typedef int ButtonCode_t;
class CPhysCollide;
class IMaterial;
struct matrix3x4_t;
struct color32;
struct OcclusionParams_t;
struct vmode_s;
class IAchievementMgr;
struct client_textmessage_t;
class CSentence;
class VMatrix;
class ISpatialQuery;
class IMaterialSystem;
struct AudioState_t;
class INetChannelInfo;
struct SkyboxVisibility_t;
class CGamestatsData;
class KeyValues;
class CGlobalVarsBase;
class CViewSetup;
struct ScreenFade_t;
class CEngineSprite;
struct ClientFrameStage_t;
class CSaveRestoreData;
struct datamap_t;
struct typedescription_t;
class CRenamedRecvTableInfo;
class IFileList;
class CMouthInfo;
class IConVar;
class ClientClass;
class CStandardRecvProxies;
struct qboolean;
struct vrect_t;

#endif /* COMMON_H_ */
