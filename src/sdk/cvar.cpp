/*
 * concommand.cpp
 *
 *  Created on: Feb 22, 2015
 *      Author: skim
 */

#include <algorithm>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> // sprintf

#include "cvar.h"
#include "ifs.h"
#include "sourceutil.h"

ConCommandBase::ConCommandBase(const char* name, const char* helpstr, int flags)
	: m_pNext(NULL), m_bRegistered(false), m_pszName(name), m_pszHelpString(helpstr), m_nFlags(flags)
{
	// Allocate the buffer for the name
	int len = strlen(name);
	m_pszName = (char*)malloc(len + 1);
	strcpy((char*)m_pszName, name);

	skim::dcon("Registering command " + std::string(name));
	skim::ifs::cvar->RegisterConCommand(this);
}
ConCommandBase::~ConCommandBase()
{
	skim::dcon("Removing command " + std::string(m_pszName));
	skim::ifs::cvar->UnregisterConCommand(this);
	free((void*)m_pszName);
}
bool ConCommandBase::IsCommand() const
{
	return true;
}
// lol why is this virtual
// The wonders of object oriented programming
bool ConCommandBase::IsFlagSet(int flag) const
{
	return m_nFlags & flag;
}
void ConCommandBase::AddFlags(int flags)
{
	m_nFlags |= flags;
}
const char* ConCommandBase::GetName() const
{
	return m_pszName;
}
const char* ConCommandBase::GetHelpText() const
{
	return m_pszHelpString;
}
bool ConCommandBase::IsRegistered() const
{
	return m_bRegistered;
}
void* ConCommandBase::GetDLLIdentifier() const
{
	return 0;
}
// Not used
void ConCommandBase::Create(const char*, const char*, int) {}
void ConCommandBase::Init() {}

ConVar::ConVar(const char *pName, const char *pDefaultValue, int flags,
		const char *pHelpString)
	: ConCommandBase(pName, pHelpString, flags),
	m_pParent(this), m_pszDefaultValue(pDefaultValue), m_pszString(NULL), m_StringLength(0),
	m_bHasMin(false), m_fMinVal(0.0f), m_bHasMax(false), m_fMaxVal(0.0f), m_fnChangeCallback(NULL)
{
	InternalSetValue(pDefaultValue);
}
ConVar::~ConVar(){}
void ConVar::AddFlags(int flags)
{
	m_nFlags |= flags;
}
const char* ConVar::GetHelpText() const
{
	return m_pszHelpString;
}
bool ConVar::IsRegistered() const
{
	return m_bRegistered;
}
void ConVar::Init() {}
void ConVar::SetValue(const char* value)
{
	m_pParent->InternalSetValue(value);
}
void ConVar::SetValue(float v)
{
	m_pParent->InternalSetFloatValue(v);
}
void ConVar::SetValue(int v)
{
	m_pParent->InternalSetIntValue(v);
}
// ConCommandBase funcs
bool ConVar::IsCommand() const
{
	return false;
}
const char* ConVar::GetName(void) const
{
	return m_pParent->m_pszName;
}
bool ConVar::IsFlagSet(int flag) const
{
	return m_pParent->m_nFlags & flag;
}
void ConVar::InternalSetValue(const char* str)
{
	// Need this for onchange later
	char* cold;
	float fold;
	if (m_fnChangeCallback)
	{
		fold = m_fValue;
		cold = (char*)malloc(m_StringLength);
		memcpy(cold, m_pszString, m_StringLength);
	}
	int len = strlen(str) + 1;

	// Re alloc if new string is larger
	int size = len;
	if (size > m_StringLength)
	{
		m_StringLength = size;
		free(m_pszString);
		m_pszString = (char*)malloc(size);
	}
	// Set new value
	memcpy(m_pszString, str, len);
	// Update integral values
	double fl = atof(str);
	m_fValue = fl;
	m_nValue = fl;

	// Notify callback
	if (m_fnChangeCallback)
	{
		m_fnChangeCallback(this, cold, fold);
		free(cold);
	}
}
void ConVar::InternalSetFloatValue(float fl)
{
	m_fValue = fl;
	m_nValue = fl;
	sprintf(m_pszString, "%f", fl);
}
void ConVar::InternalSetIntValue(int i)
{
	m_nValue = i;
	m_fValue = i;
	sprintf(m_pszString, "%i", i);
}
bool ConVar::ClampValue(float& f)
{
	float z = f;
	if (m_bHasMin)
		f = std::max(m_fMinVal, f);
	if (m_bHasMin)
		f = std::min(m_fMaxVal, f);
	return z == f;
}
void ConVar::ChangeStringValue(const char*, float) {}
void ConVar::Create(const char*, const char*, int, const char*,
		bool, float, bool, float, FnChangeCallback_t)
{}

ConCommand::ConCommand(const char *pName, FnCommandCallback_t callback,
		const char *pHelpString, int flags, FnCommandCompletionCallback completionFunc)
	: ConCommandBase(pName, pHelpString, flags),
	m_fnCommandCallback(callback), m_fnCompletionCallback(completionFunc)
{}
ConCommand::~ConCommand() {}
bool ConCommand::IsCommand() const
{
	return true;
}
int ConCommand::AutoCompleteSuggest(const char*, void*)
{
	return 0;
}
bool ConCommand::CanAutoComplete( void )
{
	return false;
}
void ConCommand::Dispatch(const CCommand &args)
{
	m_fnCommandCallback(args);
}
