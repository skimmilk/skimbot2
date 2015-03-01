/*
 * exit.h
 *
 *  Created on: Feb 28, 2015
 *      Author: skim
 */

#ifndef EXIT_H_
#define EXIT_H_

namespace skim
{

class exit
{
public:
	typedef void (*atexit_fn)();

	static void init();
	// Register exit function
	static void handle(atexit_fn);
	// Unload everything
	static void unload();
};

} /* namespace skim */

#endif /* EXIT_H_ */
