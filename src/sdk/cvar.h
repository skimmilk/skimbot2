/*
 * cvar.h
 *
 *  Created on: Feb 19, 2015
 *      Author: skim
 *
 * Purpose: Anything dealing with console variables/commands
 */

#ifndef CVAR_H_
#define CVAR_H_

#include "sdk/tf2system.h"
#include "sdk/concommand.h"
#include "sdk/common.h"

//-----------------------------------------------------------------------------
// ConVar flags
//-----------------------------------------------------------------------------
// The default, no flags at all
#define FCVAR_NONE				0

// Command to ConVars and ConCommands
// ConVar Systems
#define FCVAR_UNREGISTERED		(1<<0)	// If this is set, don't add to linked list, etc.
#define FCVAR_DEVELOPMENTONLY	(1<<1)	// Hidden in released products. Flag is removed automatically if ALLOW_DEVELOPMENT_CVARS is defined.
#define FCVAR_GAMEDLL			(1<<2)	// defined by the game DLL
#define FCVAR_CLIENTDLL			(1<<3)  // defined by the client DLL
#define FCVAR_HIDDEN			(1<<4)	// Hidden. Doesn't appear in find or autocomplete. Like DEVELOPMENTONLY, but can't be compiled out.

// ConVar only
#define FCVAR_PROTECTED			(1<<5)  // It's a server cvar, but we don't send the data since it's a password, etc.  Sends 1 if it's not bland/zero, 0 otherwise as value
#define FCVAR_SPONLY			(1<<6)  // This cvar cannot be changed by clients connected to a multiplayer server.
#define	FCVAR_ARCHIVE			(1<<7)	// set to cause it to be saved to vars.rc
#define	FCVAR_NOTIFY			(1<<8)	// notifies players when changed
#define	FCVAR_USERINFO			(1<<9)	// changes the client's info string
#define FCVAR_CHEAT				(1<<14) // Only useable in singleplayer / debug / multiplayer & sv_cheats

#define FCVAR_PRINTABLEONLY		(1<<10)  // This cvar's string cannot contain unprintable characters ( e.g., used for player name etc ).
#define FCVAR_UNLOGGED			(1<<11)  // If this is a FCVAR_SERVER, don't log changes to the log file / console if we are creating a log
#define FCVAR_NEVER_AS_STRING	(1<<12)  // never try to print that cvar

// It's a ConVar that's shared between the client and the server.
// At signon, the values of all such ConVars are sent from the server to the client (skipped for local
//  client, of course )
// If a change is requested it must come from the console (i.e., no remote client changes)
// If a value is changed while a server is active, it's replicated to all connected clients
#define FCVAR_REPLICATED		(1<<13)	// server setting enforced on clients, rename to FCAR_SERVER at some time
#define FCVAR_DEMO				(1<<16)  // record this cvar when starting a demo file
#define FCVAR_DONTRECORD		(1<<17)  // don't record these command in demofiles
#define FCVAR_RELOAD_MATERIALS	(1<<20)	// If this cvar changes, it forces a material reload
#define FCVAR_RELOAD_TEXTURES	(1<<21)	// If this cvar changes, if forces a texture reload

#define FCVAR_NOT_CONNECTED		(1<<22)	// cvar cannot be changed by a client that is connected to a server
#define FCVAR_MATERIAL_SYSTEM_THREAD (1<<23)	// Indicates this cvar is read from the material system thread
#define FCVAR_ARCHIVE_XBOX		(1<<24) // cvar written to config.cfg on the Xbox

#define FCVAR_ACCESSIBLE_FROM_THREADS	(1<<25)	// used as a debugging tool necessary to check material system thread convars

#define FCVAR_SERVER_CAN_EXECUTE	(1<<28)// the server is allowed to execute this command on clients via ClientCommand/NET_StringCmd/CBaseClientState::ProcessStringCmd.
#define FCVAR_SERVER_CANNOT_QUERY	(1<<29)// If this is set, then the server is not allowed to query this cvar's value (via IServerPluginHelpers::StartQueryCvarValue).
#define FCVAR_CLIENTCMD_CAN_EXECUTE	(1<<30)	// IVEngineClient::ClientCmd is allowed to execute this command.

class IConVar
{
public:
	// Value set
	virtual void SetValue( const char *pValue ) = 0;
	virtual void SetValue( float flValue ) = 0;
	virtual void SetValue( int nValue ) = 0;

	// Return name of command
	virtual const char *GetName( void ) const = 0;

	// Accessors.. not as efficient as using GetState()/GetInfo()
	// if you call these methods multiple times on the same IConVar
	virtual bool IsFlagSet( int nFlag ) const = 0;
};

typedef void (* FnChangeCallback_t)( IConVar* var, const char* pOldValue, float flOldValue );
//-----------------------------------------------------------------------------
// Purpose: A console variable
//-----------------------------------------------------------------------------
class ConVar : public ConCommandBase, public IConVar
{
public:
	typedef ConCommandBase BaseClass;

	ConVar(const char *pName, const char *pDefaultValue, int flags = 0,
			const char *pHelpString = 0);
	//ConVar( const char *pName, const char *pDefaultValue, int flags,
	//		const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax );

	virtual						~ConVar( void );
	virtual bool				IsFlagSet( int flag ) const;
	virtual const char*			GetHelpText( void ) const;
	virtual bool				IsRegistered( void ) const;
	virtual const char			*GetName( void ) const;
	virtual void				AddFlags( int flags );
	virtual	bool				IsCommand( void ) const;

	// Any function that allocates/frees memory needs to be virtual or else you'll have crashes
	//  from alloc/free across dll/exe boundaries.

	// These just call into the IConCommandBaseAccessor to check flags and set the var (which ends up calling InternalSetValue).
	virtual void				SetValue( const char *value );
	virtual void				SetValue( float value );
	virtual void				SetValue( int value );

	// Originally private
	// Called by CCvar when the value of a var is changing.
	virtual void				InternalSetValue(const char *value);
	// For CVARs marked FCVAR_NEVER_AS_STRING
	virtual void				InternalSetFloatValue( float fNewValue );
	virtual void				InternalSetIntValue( int nValue );

	virtual bool				ClampValue( float& value );
	virtual void				ChangeStringValue( const char *tempVal, float flOldValue );

	virtual void				Create( const char *pName, const char *pDefaultValue, int flags = 0,
			const char *pHelpString = 0, bool bMin = false, float fMin = 0.0,
			bool bMax = false, float fMax = false, FnChangeCallback_t callback = 0 );

	// Used internally by OneTimeInit to initialize.
	virtual void				Init();

	// Originally private
	// This either points to "this" or it points to the original declaration of a ConVar.
	// This allows ConVars to exist in separate modules, and they all use the first one to be declared.
	// m_pParent->m_pParent must equal m_pParent (ie: m_pParent must be the root, or original, ConVar).
	ConVar						*m_pParent;

	// Static data
	const char					*m_pszDefaultValue;

	// Value
	// Dynamically allocated
	char						*m_pszString;
	int							m_StringLength;

	// Values
	float						m_fValue;
	int							m_nValue;

	// Min/Max values
	bool						m_bHasMin;
	float						m_fMinVal;
	bool						m_bHasMax;
	float						m_fMaxVal;

	// Call this function when ConVar changes
	FnChangeCallback_t			m_fnChangeCallback;
};
#ifdef GNUC
#define FMTFUNCTION( fmtargnumber, firstvarargnumber ) __attribute__ (( format( __printf__, fmtargnumber, firstvarargnumber )))
#else
#define FMTFUNCTION( a, b )
#endif
//-----------------------------------------------------------------------------
// Used to display console messages
//-----------------------------------------------------------------------------
class IConsoleDisplayFunc
{
public:
	virtual void ColorPrint( const color& clr, const char *pMessage ) = 0;
	virtual void Print( const char *pMessage ) = 0;
	virtual void DPrint( const char *pMessage ) = 0;
};
//-----------------------------------------------------------------------------
// Purpose: DLL interface to ConVars/ConCommands
//-----------------------------------------------------------------------------
class ICvar : public IAppSystem
{
public:
	// Allocate a unique DLL identifier
	virtual void* AllocateDLLIdentifier() = 0;

	// Register, unregister commands
	virtual void			RegisterConCommand( ConCommandBase *pCommandBase ) = 0;
	virtual void			UnregisterConCommand( ConCommandBase *pCommandBase ) = 0;
	// Pass DLL identifier
	virtual void			UnregisterConCommands( void* id ) = 0;

	// If there is a +<varname> <value> on the command line, this returns the value.
	// Otherwise, it returns NULL.
	virtual const char*		GetCommandLineValue( const char *pVariableName ) = 0;

	// Try to find the cvar pointer by name
	virtual ConCommandBase *FindCommandBase( const char *name ) = 0;
	virtual const ConCommandBase *FindCommandBase( const char *name ) const = 0;
	virtual ConVar			*FindVar ( const char *var_name ) = 0;
	virtual const ConVar	*FindVar ( const char *var_name ) const = 0;
	virtual ConCommand		*FindCommand( const char *name ) = 0;
	virtual const ConCommand *FindCommand( const char *name ) const = 0;

	// Get first ConCommandBase to allow iteration
	virtual ConCommandBase	*GetCommands( void ) = 0;
	virtual const ConCommandBase *GetCommands( void ) const = 0;

	// Install a global change callback (to be called when any convar changes)
	virtual void			InstallGlobalChangeCallback( FnChangeCallback_t callback ) = 0;
	virtual void			RemoveGlobalChangeCallback( FnChangeCallback_t callback ) = 0;
	virtual void			CallGlobalChangeCallbacks( ConVar *var, const char *pOldString, float flOldValue ) = 0;

	// Install a console printer
	virtual void			InstallConsoleDisplayFunc( IConsoleDisplayFunc* pDisplayFunc ) = 0;
	virtual void			RemoveConsoleDisplayFunc( IConsoleDisplayFunc* pDisplayFunc ) = 0;
	virtual void			ConsoleColorPrintf( const color& clr, const char *pFormat, ... ) const FMTFUNCTION( 3, 4 ) = 0;
	virtual void			ConsolePrintf( const char *pFormat, ... ) const FMTFUNCTION( 2, 3 ) = 0;
	virtual void			ConsoleDPrintf( const char *pFormat, ... ) const FMTFUNCTION( 2, 3 ) = 0;

	// Reverts cvars which contain a specific flag
	virtual void			RevertFlaggedConVars( int nFlag ) = 0;

	// Method allowing the engine ICvarQuery interface to take over
	// A little hacky, owing to the fact the engine is loaded
	// well after ICVar, so we can't use the standard connect pattern
	virtual void			InstallCVarQuery( void* pQuery ) = 0;

#if defined( _X360 )
	virtual void			PublishToVXConsole( ) = 0;
#endif
	virtual bool			IsMaterialThreadSetAllowed( ) const = 0;
	virtual void			QueueMaterialThreadSetValue( ConVar *pConVar, const char *pValue ) = 0;
	virtual void			QueueMaterialThreadSetValue( ConVar *pConVar, int nValue ) = 0;
	virtual void			QueueMaterialThreadSetValue( ConVar *pConVar, float flValue ) = 0;
	virtual bool			HasQueuedMaterialThreadConVarSets() const = 0;
	virtual int				ProcessQueuedMaterialThreadConVarSets() = 0;

protected:	class ICVarIteratorInternal;
public:
	/// Iteration over all cvars.
	/// (THIS IS A SLOW OPERATION AND YOU SHOULD AVOID IT.)
	/// usage:
	/// { ICVar::Iterator iter(g_pCVar);
	///   for ( iter.SetFirst() ; iter.IsValid() ; iter.Next() )
	///   {
	///       ConCommandBase *cmd = iter.Get();
	///   }
	/// }
	/// The Iterator class actually wraps the internal factory methods
	/// so you don't need to worry about new/delete -- scope takes care
	//  of it.
	/// We need an iterator like this because we can't simply return a
	/// pointer to the internal data type that contains the cvars --
	/// it's a custom, protected class with unusual semantics and is
	/// prone to change.
	class Iterator
	{
	public:
		inline Iterator(ICvar *icvar);
		inline ~Iterator(void);
		inline void		SetFirst( void );
		inline void		Next( void );
		inline bool		IsValid( void );
		inline ConCommandBase *Get( void );
	private:
		ICVarIteratorInternal *m_pIter;
	};

protected:
	// internals for  ICVarIterator
	class ICVarIteratorInternal
	{
	public:
		// warning: delete called on 'ICvar::ICVarIteratorInternal' that is abstract but has non-virtual destructor [-Wdelete-non-virtual-dtor]
		virtual ~ICVarIteratorInternal() {}
		virtual void		SetFirst( void ) = 0;
		virtual void		Next( void ) = 0;
		virtual	bool		IsValid( void ) = 0;
		virtual ConCommandBase *Get( void ) = 0;
	};

	virtual ICVarIteratorInternal	*FactoryInternalIterator( void ) = 0;
	friend class Iterator;
};

#endif /* CVAR_H_ */
