/*
 * basehook.h
 *
 *  Created on: Feb 27, 2015
 *      Author: skim
 */

#ifndef BASEHOOK_H_
#define BASEHOOK_H_

#include <string>
#include "sdk/usercmd.h"

namespace skim
{

// int sequence_number, float input_sample_frametime, bool active
typedef void (*premove_fn)(int&, float&, bool&);
typedef void (*postmove_fn)(CUserCmd*);
typedef void (*paint_fn)();

class basehook
{
public:
	static void init();
	static void unload();

	// Call passed function on hook
	static void pre_move(premove_fn, const std::string& hackname);
	static void post_move(postmove_fn, const std::string& hackname);
	static void post_paint(paint_fn, const std::string& hackname);
};

} /* namespace skim */

#endif /* BASEHOOK_H_ */
