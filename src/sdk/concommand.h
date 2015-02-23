/*
 * concommand.h
 *
 *  Created on: Feb 19, 2015
 *      Author: skim
 */

#ifndef CONCOMMAND_H_
#define CONCOMMAND_H_


class ConCommandBase
{
public:
	ConCommandBase(const char* name, const char* helpstr, int flags);
	virtual						~ConCommandBase( void );
	virtual	bool				IsCommand( void ) const;
	// Check flag
	virtual bool				IsFlagSet( int flag ) const;
	// Set flag
	virtual void				AddFlags( int flags );
	// Return name of cvar
	virtual const char			*GetName( void ) const;
	// Return help text for cvar
	virtual const char			*GetHelpText( void ) const;
	virtual bool				IsRegistered( void ) const;
	// Returns the DLL identifier
	virtual void* GetDLLIdentifier() const;

	// Originally protected
	virtual void				Create( const char *pName, const char *pHelpString = 0,
			int flags = 0 );
	// Used internally by OneTimeInit to initialize/shutdown
	virtual void				Init();

	// Originally private
	// Next ConVar in chain
	// Prior to register, it points to the next convar in the DLL.
	// Once registered, though, m_pNext is reset to point to the next
	// convar in the global list
	ConCommandBase				*m_pNext;

	// Has the cvar been added to the global list?
	bool						m_bRegistered;
	// Static data
	const char 					*m_pszName;
	const char 					*m_pszHelpString;
	// ConVar flags
	int							m_nFlags;

	/*protected:
	// ConVars add themselves to this list for the executable.
	// Then ConVar_Register runs through  all the console variables
	// and registers them into a global list stored in vstdlib.dll
	static ConCommandBase		*s_pConCommandBases;

	// ConVars in this executable use this 'global' to access values.
	static IConCommandBaseAccessor	*s_pAccessor;*/
};

//-----------------------------------------------------------------------------
// Command tokenizer
//-----------------------------------------------------------------------------
class CCommand
{
public:
	inline const char* Arg(int) const;

	// Originally private
	enum
	{
		COMMAND_MAX_ARGC = 64,
		COMMAND_MAX_LENGTH = 512,
	};

	int		m_nArgc;
	int		m_nArgv0Size;
	char	m_pArgSBuffer[ COMMAND_MAX_LENGTH ];
	char	m_pArgvBuffer[ COMMAND_MAX_LENGTH ];
	const char*	m_ppArgv[ COMMAND_MAX_ARGC ];
};
inline const char *CCommand::Arg( int nIndex ) const
{
	if ( nIndex < 0 || nIndex >= m_nArgc )
		return "";
	return m_ppArgv[nIndex];
}
typedef void ( *FnCommandCallbackVoid_t )( void );
typedef void ( *FnCommandCallback_t )( const CCommand &command );
typedef int  ( *FnCommandCompletionCallback )( const char *partial, char** commands );
//-----------------------------------------------------------------------------
// Purpose: The console invoked command
//-----------------------------------------------------------------------------
class ConCommand : public ConCommandBase
{
public:
	typedef ConCommandBase BaseClass;
	ConCommand( const char *pName, FnCommandCallbackVoid_t callback,
			const char *pHelpString = 0, int flags = 0, FnCommandCompletionCallback completionFunc = 0 );
	ConCommand( const char *pName, FnCommandCallback_t callback,
			const char *pHelpString = 0, int flags = 0, FnCommandCompletionCallback completionFunc = 0 );

	virtual ~ConCommand( void );
	virtual	bool IsCommand( void ) const;
	virtual int AutoCompleteSuggest( const char *partial, void* );
	virtual bool CanAutoComplete( void );
	// Invoke the function
	virtual void Dispatch( const CCommand &command );

	// Originally private
	// NOTE: To maintain backward compat, we have to be very careful:
	// All public virtual methods must appear in the same order always
	// since engine code will be calling into this code, which *does not match*
	// in the mod code; it's using slightly different, but compatible versions
	// of this class. Also: Be very careful about adding new fields to this class.
	// Those fields will not exist in the version of this class that is instanced
	// in mod code.

	// Call this function when executing the command
	union
	{
		FnCommandCallbackVoid_t m_fnCommandCallbackV1;
		FnCommandCallback_t m_fnCommandCallback;
		//ICommandCallback *m_pCommandCallback;
	};

	// Completion callback
	FnCommandCompletionCallback	m_fnCompletionCallback;

	bool m_bHasCompletionCallback : 1;
	bool m_bUsingNewCommandCallback : 1;
	bool m_bUsingCommandCallbackInterface : 1;
};


#endif /* CONCOMMAND_H_ */
