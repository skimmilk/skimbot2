/*
 * ifs.h
 *
 *  Created on: Feb 22, 2015
 *      Author: skim
 */

#ifndef IFS_H_
#define IFS_H_

class IBaseClientDLL;
class IVEngineClient;
class IEngineTrace;
class ICvar;
class IVModelInfoClient;
class IClientEntityList;
class IPanel;
class ISurface;
class IEngineVGui;
class IGameEventManager2;
class IPrediction;
class IStudioRender;
class CGlobalVarsBase;
class IVRenderView;
class IViewRender;
class IMaterialSystem;
class IVModelRender;
class IVDebugOverlay;
class IInput;
class IGameResources;
class IClientEntity;

class IAchievementMgr;
class CAchievementMgr;
class ISteamUserStats;
class ISteamFriends;
class ISteamClient;

namespace skim
{

class ifs
{
public:
	static IBaseClientDLL*		client;
	static IVEngineClient*		engine;
	static IEngineTrace*		tracer;
	static ICvar*				cvar;
	static IVModelInfoClient*	model_info;
	static IClientEntityList*	entities;
	static IPanel*				panel;
	static ISurface*			surface;
	static IEngineVGui*			vgui;
	static IGameEventManager2*	event;
	static IPrediction*			prediction;
	static IStudioRender*		studio_render;
	static CGlobalVarsBase*		globals;
	static IVRenderView*		render_view;
	static IViewRender*			view_render;
	static IMaterialSystem*		materials;
	static IVModelRender*		model_render;
	static IVDebugOverlay*		overlay;
	static IGameResources*		(*resources)();
	static IClientEntity*		(*player)();
	static IInput*				input;

	static IAchievementMgr*		achievements;
	static ISteamUserStats*		(*steam_stats)();
	static ISteamFriends*		(*steam_friends)();
	static ISteamClient*		(*steam_client)();

	static bool load();
};

} /* namespace skim */

#endif /* IFS_H_ */
