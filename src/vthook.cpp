/*
 * vthook.cpp
 *
 *  Created on: Feb 26, 2015
 *      Author: skim
 */

#include "vthook.h"

#include <string.h>

namespace skim
{

long* hook_vmt(void* obj, long* vmtholder, int num_fns, int fn_index, void* fn)
{
    // Grab the vtable pointer
    long** pptable = (long**)obj;
    // Grab the vtable
    long* result = *pptable;

    if (*pptable != vmtholder)
    {
        memcpy(vmtholder, *pptable, num_fns * sizeof(long));
        // Set the vtable pointer inside object to the copied vmt
        *pptable = vmtholder;
    }

    vmtholder[fn_index] = (long)fn;
    return result;
}

} /* namespace skim */
