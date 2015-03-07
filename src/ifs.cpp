/*
 * ifs.cpp
 *
 *  Created on: Feb 22, 2015
 *      Author: skim
 */

#include "ifs.h"

#include <thread>
#include <chrono>
#include <dlfcn.h>
#include <stdlib.h>

#include "sdk/engine.h"
#include "sdk/steam.h"
#include "sigscan.h"
#include "debug.h"

namespace skim
{
IBaseClientDLL*		ifs::client;// client
IVEngineClient*		ifs::engine;// engine
IEngineTrace*		ifs::tracer;// engine
ICvar*				ifs::cvar;// engine
IVModelInfoClient*	ifs::model_info;// engine
IClientEntityList*	ifs::entities;// client
IPanel*				ifs::panel;// vgui
ISurface*			ifs::surface;// vgui
IEngineVGui*		ifs::vgui;// engine
IGameEventManager2*	ifs::event;// engine
IPrediction*		ifs::prediction;// engine
IStudioRender*		ifs::studio_render;// vgui
CGlobalVarsBase*	ifs::globals;
IVRenderView*		ifs::render_view;// engine
IViewRender*		ifs::view_render;
IMaterialSystem*	ifs::materials;// if
IVModelRender*		ifs::model_render;// engine
IVDebugOverlay*		ifs::overlay;// engine
IInput*				ifs::input;
IGameResources*		(*ifs::resources)();
IClientEntity*		(*ifs::player)();

IAchievementMgr*	ifs::achievements;
ISteamUserStats*	ifs::steam_stats;
ISteamFriends*		ifs::steam_friends;
ISteamClient*		ifs::steam_client;

template <typename T>
static void* set(CreateInterfaceFn cfn, T*& a, const char* ifname)
{
	a = (T*)cfn(ifname, 0);
	return (void*)a;
}

// Wait for client to get loaded
static void* waitfor(const char* directory)
{
	void* result = 0;
	while ((result = dlopen(directory, RTLD_NOLOAD)) == 0)
		std::this_thread::sleep_for(std::chrono::milliseconds (250));
	return result;
}
#define CHK(f, v, s) if (!set(f, v, s) && !window("Failed to acquire " s ", continue?")) return false;

static bool load_steam(CreateInterfaceFn interface_factory)
{
	if (false && window("Open steam?"))
	{
		// Open up steam
		CHK(interface_factory, ifs::steam_client, "SteamClient016");

		window("Opening steam pipe");
		HSteamPipe pipe = ifs::steam_client->CreateSteamPipe();
		if (!pipe) window("Could not get steam pipe");

		window("Opening steam user");
		HSteamUser user = ifs::steam_client->ConnectToGlobalUser(pipe);
		if (!user) window("Could not get steam user");

		window("Opening steam friends");
		ifs::steam_friends = ifs::steam_client->GetISteamFriends(user, pipe,
				"STEAMUSERSTATS_INTERFACE_VERSION014");
		if (!ifs::steam_friends) window("Could not get steam friends interface");
	}
	return true;
}
static bool load_client(const std::string& directory)
{
	void* client_handle = waitfor(directory.c_str());
	if (!client_handle)
	{
		window("Failied to open client library");
		return false;
	}

	CreateInterfaceFn client_factory = (CreateInterfaceFn)dlsym(client_handle, "CreateInterface");
	CHK(client_factory, ifs::client, "VClient017");
	CHK(client_factory, ifs::entities, "VClientEntityList003");

	return true;
}
static bool load_gameres(const std::string& directory)
{
	// The string 'deaths' and 'frags' will get you here
	// Lands you in GameResources()
	char*  p = (char*)sigscan(directory, "55  89 E5  85 C0  74 0C  05 3C 05 00 00  5D  C3");
	if (p)
		ifs::resources = (IGameResources* (*)())(p - 5);
	else
	{
		window("Failed to acquire game resources");
		return false;
	}
	return true;
}
static bool load_getplayer(const std::string& directory)
{
	// The string Player.WeaponSelectionClose will get you here
	char* p = (char*)sigscan(directory, "85 C0  89 C6  74 21  8B 03  89 1C 24  FF 50 28");
	if (p)
		ifs::player = (IClientEntity* (*)())(*(long*)(p - 4));
	else
	{
		window("Failed to acquire local player");
		return false;
	}
	return true;
}
static bool load_input()
{
	// Client::CreateMove + 61 = location of IInput
	// Get the vtable of ifs::client
	long* client_vmt = *(long**)ifs::client;

	// Get the function pointer of client->CreateMove
	long* createmovefn = (long*)client_vmt[21];

	// Get the pointer to IInput
	ifs::input = **(IInput***)((char*)createmovefn + 61);

	return true;
}
static bool load_surface(const std::string& directory)
{
	// CClientReplayImp::PlaySound() references ISurface
	// CClientReplayImp can be found with the string "cancelled" in client.so
	// cdll_replay.cpp
	char* offset = (char*)skim::sigscan(directory,
			"55  89 E5  85 C0  74 14  8B 08  89 45 08  5D  8B 81 38 01 00 00");
	if (!offset)
	{
		window("Could not locate surface");
		return false;
	}

	ifs::surface = **(ISurface***)(offset - 4);
	return true;
}
static bool load_globals(const std::string& directory)
{
	// String "passedresult" will get you here in CHudVote::MsgFunc_VotePass
	char* offset = (char*)skim::sigscan(directory,
			"FF 91 90 03 00 00  A1");
	if (!offset)
	{
		window("Could not locate CGlobalVarsBase");
		return false;
	}
	offset += 7;
	ifs::globals = **(CGlobalVarsBase***)offset;
	return true;
}

bool ifs::load()
{
	// Get the AppSystem interface function, in engine.so
	// The string VGUI_InputInternal001 will get you this sig
	long** offset = (long**)skim::sigscan("engine.so",
			"55  89 E5  83 EC 28  89 75 F8  8B 75 08"
			"89 7D FC  8B 7D 0C  89 5D F4  89 34 24  89 7C 24 04");

	CreateInterfaceFn interface_factory = **(CreateInterfaceFn**)((char*)offset + 30);

	CHK(interface_factory, ifs::cvar, "VEngineCvar004")
	CHK(interface_factory, ifs::event, "GAMEEVENTSMANAGER002");
	CHK(interface_factory, ifs::materials, "VMaterialSystem080");
	CHK(interface_factory, ifs::engine, "VEngineClient014");
	CHK(interface_factory, ifs::vgui, "VEngineVGui001");
	CHK(interface_factory, ifs::model_info, "VModelInfoClient006");
	CHK(interface_factory, ifs::model_render, "VEngineModel016");
	CHK(interface_factory, ifs::overlay, "VDebugOverlay003");
	CHK(interface_factory, ifs::render_view, "VEngineRenderView014");
	CHK(interface_factory, ifs::tracer, "EngineTraceClient003");
	//CHK(interface_factory, ifs::prediction, "VClientPrediction001");

	CHK(interface_factory, ifs::panel, "VGUI_Panel009");
	//CHK(interface_factory, ifs::surface, "VGUI_Surface034");
	CHK(interface_factory, ifs::studio_render, "VStudioRender025");

	if (!load_steam(interface_factory))
		return false;

	// The client library takes a while to load
	std::string directory (ifs::engine->GetGameDirectory());
	directory += "/bin/client.so";

	if (!load_client(directory) ||
			!load_gameres(directory) ||
			!load_getplayer(directory) ||
			!load_input() ||
			!load_surface(directory) ||
			!load_globals(directory))
		return false;

	return true;
}
} /* namespace skim */
