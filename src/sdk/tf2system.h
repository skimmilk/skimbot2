/*
 * tf2system.h
 *
 *  Created on: Feb 18, 2015
 *      Author: skim
 *
 * Use: Internal game mechanisms
 */

#ifndef TF2SYSTEM_H_
#define TF2SYSTEM_H_

#include "common.h"

class IAppSystem
{
public:
	// Here's where the app systems get to learn about each other
	virtual bool Connect( CreateInterfaceFn factory ) = 0;
	virtual void Disconnect() = 0;

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void *QueryInterface( const char *pInterfaceName ) = 0;

	// Init, shutdown
	virtual void* Init() = 0;
	virtual void Shutdown() = 0;
};



#endif /* TF2SYSTEM_H_ */
