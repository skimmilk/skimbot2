/*
 * netvar.cpp
 *
 *  Created on: Feb 23, 2015
 *      Author: skim
 */

#include "netvar.h"

#include <vector>
#include <string.h>
#include "ifs.h"
#include "sdk/client.h"
#include "debug.h"

namespace skim
{

// Recursively search a table for its elements
RecvProp* netsearch(RecvTable* table, const char* offname)
{
	for (int i = 0; i < table->m_nProps; i++)
	{
		if (!strcmp(table->m_pProps[i].m_pVarName, offname))
			return table->m_pProps + i;
		if (!strcmp(table->m_pProps[i].m_pVarName, "baseclass"))
		{
			RecvProp* inner = netsearch(table->m_pProps[i].m_pDataTable, offname);
			if (inner)
				return inner;
		}
	}
	return nullptr;
}

// Search a class for variables
int netvar::netoffset(const char* classname, const std::vector<const char*>& vars, RecvProp** prop)
{
	int offset = 0;
	ClientClass* cclass = nullptr;

	// Find the class by name
	for (cclass = skim::ifs::client->GetAllClasses(); cclass; cclass = cclass->m_pNext)
		if (!strcmp(cclass->m_pNetworkName, classname))
			break;

	if (cclass == nullptr)
		return 0;

	// Found the correct class
	RecvTable* table = cclass->m_pRecvTable;
	RecvProp* result;

	// Combine the offsets in vars
	for (uint i = 0; i < vars.size(); i++)
	{
		result = netsearch(table, vars[i]);
		if (!result)
			return 0;

		offset += result->m_Offset;
	}

	if (prop)
		*prop = result;
	return offset;
}

int netvar::netoffset(const char* classname, const char* varname, RecvProp** prop)
{
	return netoffset(classname, std::vector<const char*>{varname}, prop);
}

} /* namespace skim */
