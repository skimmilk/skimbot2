/*
 * recv.h
 *
 *  Created on: Feb 19, 2015
 *      Author: skim
 */

#ifndef RECV_H_
#define RECV_H_

enum SendPropType
{
	DPT_INT,
	DPT_FLOAT,
	DPT_VECTOR,
	DPT_STRING,
	DPT_ARRAY,
	DPT_DATATABLE,
};

struct DVariant
{
	union
	{
		float Float;
		long Int;
		char* pString;
		void* pData;	// For DataTables.
		float Vector[3];
	};
	SendPropType Type;
};

class RecvProp;
// This is passed into RecvProxy functions.
class CRecvProxyData
{
public:
	const RecvProp *m_pRecvProp; // The property it's receiving.
	DVariant m_Value; // The value given to you to store.
	int	m_iElement; // Which array element you're getting.
	int	m_ObjectID; // The object being referred to.
};

//-----------------------------------------------------------------------------
// pStruct = the base structure of the datatable this variable is in (like C_BaseEntity)
// pOut = the variable that this this proxy represents (like C_BaseEntity::m_SomeValue).
//
// Convert the network-standard-type value in m_Value into your own format in pStruct/pOut.
//-----------------------------------------------------------------------------
typedef void (*RecvVarProxyFn)( const CRecvProxyData *pData, void *pStruct, void *pOut );

// ------------------------------------------------------------------------ //
// ArrayLengthRecvProxies are optionally used to get the length of the
// incoming array when it changes.
// ------------------------------------------------------------------------ //
typedef void (*ArrayLengthRecvProxyFn)( void *pStruct, int objectID, int currentArrayLength );

// NOTE: DataTable receive proxies work differently than the other proxies.
// pData points at the object + the recv table's offset.
// pOut should be set to the location of the object to unpack the data table into.
// If the parent object just contains the child object, the default proxy just does *pOut = pData.
// If the parent object points at the child object, you need to dereference the pointer here.
// NOTE: don't ever return null from a DataTable receive proxy function. Bad things will happen.
typedef void (*DataTableRecvVarProxyFn)(const RecvProp *pProp, void **pOut, void *pData, int objectID);
class RecvTable;

class RecvProp
{
	// This info comes from the receive data table.
public:
	const char              *m_pVarName;
	SendPropType			m_RecvType;
	int						m_Flags;
	int						m_StringBufferSize;

	// Originally private members
	bool					m_bInsideArray;		// Set to true by the engine if this property sits inside an array.

	// Extra data that certain special property types bind to the property here.
	const void *m_pExtraData;

	// If this is an array (DPT_Array).
	RecvProp				*m_pArrayProp;
	ArrayLengthRecvProxyFn	m_ArrayLengthProxy;

	RecvVarProxyFn			m_ProxyFn;
	DataTableRecvVarProxyFn	m_DataTableProxyFn;	// For RDT_DataTable.

	RecvTable				*m_pDataTable;		// For RDT_DataTable.
	int						m_Offset;

	int						m_ElementStride;
	int						m_nElements;

	// If it's one of the numbered "000", "001", etc properties in an array, then
	// these can be used to get its array property name for debugging.
	const char				*m_pParentArrayPropName;
};


class RecvTable
{
public:
	// Properties described in a table.
	RecvProp		*m_pProps;
	int				m_nProps;

	// The decoder. NOTE: this covers each RecvTable AND all its children (ie: its children
	// will have their own decoders that include props for all their children).
	/*CRecvDecoder*/ void	*m_pDecoder;

	const char		*m_pNetTableName;	// The name matched between client and server.

	// Originally private
	bool			m_bInitialized;
	bool			m_bInMainList;
};

class IClientNetworkable;
typedef IClientNetworkable* (*CreateClientClassFn)( int entnum, int serialNum );
typedef IClientNetworkable* (*CreateEventFn)();

class ClientClass
{
public:
	CreateClientClassFn m_pCreateFn;
	CreateEventFn m_pCreateEventFn; // Only called for event objects.
	const char	*m_pNetworkName;
	RecvTable *m_pRecvTable;
	ClientClass *m_pNext;
	int	m_ClassID; // Managed by the engine.
};

#endif /* RECV_H_ */
