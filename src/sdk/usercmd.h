/*
 * usercmd.h
 *
 *  Created on: Feb 27, 2015
 *      Author: skim
 */

#ifndef USERCMD_H_
#define USERCMD_H_

#include "crc.h"
#include "common.h"

class bf_read;
class bf_write;

// Buttons
#define IN_ATTACK		(1 << 0)
#define IN_JUMP			(1 << 1)
#define IN_DUCK			(1 << 2)
#define IN_FORWARD		(1 << 3)
#define IN_BACK			(1 << 4)
#define IN_USE			(1 << 5)
#define IN_CANCEL		(1 << 6)
#define IN_LEFT			(1 << 7)
#define IN_RIGHT		(1 << 8)
#define IN_MOVELEFT		(1 << 9)
#define IN_MOVERIGHT	(1 << 10)
#define IN_ATTACK2		(1 << 11)
#define IN_RUN			(1 << 12)
#define IN_RELOAD		(1 << 13)
#define IN_ALT1			(1 << 14)
#define IN_ALT2			(1 << 15)
#define IN_SCORE		(1 << 16)   // Used by client.dll for when scoreboard is held down
#define IN_SPEED		(1 << 17)	// Player is holding the speed key
#define IN_WALK			(1 << 18)	// Player holding walk key
#define IN_ZOOM			(1 << 19)	// Zoom key for HUD zoom
#define IN_WEAPON1		(1 << 20)	// weapon defines these bits
#define IN_WEAPON2		(1 << 21)	// weapon defines these bits
#define IN_BULLRUSH		(1 << 22)
#define IN_GRENADE1		(1 << 23)	// grenade 1
#define IN_GRENADE2		(1 << 24)	// grenade 2
#define	IN_ATTACK3		(1 << 25)

#define	FL_ONGROUND				(1<<0)	// At rest / on the ground
#define FL_DUCKING				(1<<1)	// Player flag -- Player is fully crouched
#define	FL_WATERJUMP			(1<<2)	// player jumping out of water
#define FL_ONTRAIN				(1<<3) // Player is _controlling_ a train, so movement commands should be ignored on client during prediction.
#define FL_INRAIN				(1<<4)	// Indicates the entity is standing in rain
#define FL_FROZEN				(1<<5) // Player is frozen for 3rd person camera
#define FL_ATCONTROLS			(1<<6) // Player can't move, but keeps key inputs for controlling another entity
#define	FL_CLIENT				(1<<7)	// Is a player
#define FL_FAKECLIENT			(1<<8)	// Fake client, simulated server side; don't send network messages to them
// NON-PLAYER SPECIFIC (i.e., not used by GameMovement or the client .dll ) -- Can still be applied to players, though
#define	FL_INWATER				(1<<9)	// In water

// NOTE if you move things up, make sure to change this value
#define PLAYER_FLAG_BITS		10

#define	FL_FLY					(1<<10)	// Changes the SV_Movestep() behavior to not need to be on ground
#define	FL_SWIM					(1<<11)	// Changes the SV_Movestep() behavior to not need to be on ground (but stay in water)
#define	FL_CONVEYOR				(1<<12)
#define	FL_NPC					(1<<13)
#define	FL_GODMODE				(1<<14)
#define	FL_NOTARGET				(1<<15)
#define	FL_AIMTARGET			(1<<16)	// set if the crosshair needs to aim onto the entity
#define	FL_PARTIALGROUND		(1<<17)	// not all corners are valid
#define FL_STATICPROP			(1<<18)	// Eetsa static prop!
#define FL_GRAPHED				(1<<19) // worldgraph has this ent listed as something that blocks a connection
#define FL_GRENADE				(1<<20)
#define FL_STEPMOVEMENT			(1<<21)	// Changes the SV_Movestep() behavior to not do any processing
#define FL_DONTTOUCH			(1<<22)	// Doesn't generate touch functions, generates Untouch() for anything it was touching when this flag was set
#define FL_BASEVELOCITY			(1<<23)	// Base velocity has been applied this frame (used to convert base velocity into momentum)
#define FL_WORLDBRUSH			(1<<24)	// Not moveable/removeable brush entity (really part of the world, but represented as an entity for transparency or something)
#define FL_OBJECT				(1<<25) // Terrible name. This is an object that NPCs should see. Missiles, for example.
#define FL_KILLME				(1<<26)	// This entity is marked for death -- will be freed by game DLL
#define FL_ONFIRE				(1<<27)	// You know...
#define FL_DISSOLVING			(1<<28) // We're dissolving!
#define FL_TRANSRAGDOLL			(1<<29) // In the process of turning into a client side ragdoll.
#define FL_UNBLOCKABLE_BY_PLAYER (1<<30) // pusher that can't be blocked by the player

class CUserCmd
{
public:
	CUserCmd& operator =( const CUserCmd& src )
	{
		if ( this == &src )
			return *this;

		command_number		= src.command_number;
		tick_count			= src.tick_count;
		viewangles			= src.viewangles;
		forwardmove			= src.forwardmove;
		sidemove			= src.sidemove;
		upmove				= src.upmove;
		buttons				= src.buttons;
		impulse				= src.impulse;
		weaponselect		= src.weaponselect;
		weaponsubtype		= src.weaponsubtype;
		random_seed			= src.random_seed;
		mousedx				= src.mousedx;
		mousedy				= src.mousedy;

		hasbeenpredicted	= src.hasbeenpredicted;

		return *this;
	}

	CRC32_t GetChecksum( void ) const
	{
		CRC32_t crc;

		CRC32_Init( &crc );
		CRC32_ProcessBuffer( &crc, &command_number, sizeof( command_number ) );
		CRC32_ProcessBuffer( &crc, &tick_count, sizeof( tick_count ) );
		CRC32_ProcessBuffer( &crc, &viewangles, sizeof( viewangles ) );
		CRC32_ProcessBuffer( &crc, &forwardmove, sizeof( forwardmove ) );
		CRC32_ProcessBuffer( &crc, &sidemove, sizeof( sidemove ) );
		CRC32_ProcessBuffer( &crc, &upmove, sizeof( upmove ) );
		CRC32_ProcessBuffer( &crc, &buttons, sizeof( buttons ) );
		CRC32_ProcessBuffer( &crc, &impulse, sizeof( impulse ) );
		CRC32_ProcessBuffer( &crc, &weaponselect, sizeof( weaponselect ) );
		CRC32_ProcessBuffer( &crc, &weaponsubtype, sizeof( weaponsubtype ) );
		CRC32_ProcessBuffer( &crc, &random_seed, sizeof( random_seed ) );
		CRC32_ProcessBuffer( &crc, &mousedx, sizeof( mousedx ) );
		CRC32_ProcessBuffer( &crc, &mousedy, sizeof( mousedy ) );
		CRC32_Final( &crc );

		return crc;
	}

	virtual ~CUserCmd() {}

	// For matching server and client commands for debugging
	int		command_number;

	// the tick the client created this command
	int		tick_count;

	// Player instantaneous view angles.
	QAngle	viewangles;
	// Intended velocities
	//	forward velocity.
	float	forwardmove;
	//  sideways velocity.
	float	sidemove;
	//  upward velocity.
	float	upmove;
	// Attack button states
	int		buttons;
	// Impulse command issued.
	/*byte*/char    impulse;
	// Current weapon id
	int		weaponselect;
	int		weaponsubtype;

	int		random_seed;	// For shared random functions

	short	mousedx;		// mouse accum in x from create move
	short	mousedy;		// mouse accum in y from create move

	// Client only, tracks whether we've predicted this command at least once
	bool	hasbeenpredicted;
};


#endif /* USERCMD_H_ */
