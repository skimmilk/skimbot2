/*
 * vthook.h
 *
 *  Created on: Feb 26, 2015
 *      Author: skim
 */

#ifndef VTHOOK_H_
#define VTHOOK_H_

namespace skim
{

// obj is the pointer to the object to be modified
// vmtholder is the pointer to the array that will be used as the new vtable
// num_fns is the number of functions in obj's vtable
// fn_index is the index of the function to hook in the vtable
// fn is the function that will get called
long* hook_vmt(void* obj, long* vmtholder, int num_fns, int fn_index, void* fn);

// Does the casting for us
template <class T, typename R, typename... Args>
long* hook_vmt(T* obj, long* vmtholder, int num_fns, int fn_index, R (*fn)(Args...))
{
	return hook_vmt((void*)obj, vmtholder, num_fns, fn_index, (void*)fn);
}

// The vthook object supports hooking one function in a class
template <int num_fns>
class vthook
{
private:
	long newvmt[num_fns];
	long* origvmt;
	void* obj;

public:
	~vthook()
	{
		unhook();
	}

	inline void hook(void* _obj, int fn_index, void* fn)
	{
		obj = _obj;
		origvmt = hook_vmt(_obj, newvmt, num_fns, fn_index, fn);
	}

	template <class T, typename R, typename... Args>
	inline void hook(T* _obj, int fn_index, R (*fn)(Args...))
	{
		hook((void*)_obj, fn_index, (void*)fn);
	}

	// Set vmt back to its original
	inline void unhook()
	{
	    long** pptable = (long**)obj;
	    *pptable = origvmt;
	}

	// Set vmt to the modified table
	inline void rehook()
	{
	    long** pptable = (long**)obj;
	    *pptable = newvmt;
	}
};

} /* namespace skim */

#endif /* VTHOOK_H_ */
