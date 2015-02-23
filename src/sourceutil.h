/*
 * sourceutil.h
 *
 *  Created on: Feb 22, 2015
 *      Author: skim
 */

#ifndef SOURCEUTIL_H_
#define SOURCEUTIL_H_

#include <string>

#include "sdk/common.h"

namespace skim
{

// Write the string to the developer console
void con(const std::string& a);

// Write the string to the developer console if developer mode on
void dcon(const std::string& a);

// Write the string to the developer console, error
void econ(const std::string& a);

// Write the string to the developer console as color c
void ccon(const color& c, const std::string& a);

} /* namespace skim */

#endif /* SOURCEUTIL_H_ */
