/*
 * sigscan.h
 *
 *  Created on: Feb 22, 2015
 *      Author: skim
 */

#ifndef SIGSCAN_H_
#define SIGSCAN_H_

#include <string>

namespace skim
{

void* sigscan(const std::string& libname, const std::string& signature);

} /* namespace skim */

#endif /* SIGSCAN_H_ */
