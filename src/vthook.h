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

// The vthook object supports hooking one function in a class
class vthook
{
public:
	// obj is the pointer to the object to be modified
	// vmtholder is the pointer to the array that will be used as the new vtable
	// num_fns is the number of functions in obj's vtable
	// fn_index is the index of the function to hook in the vtable
	// fn is the function that will get called
	static long* hook(void* obj, long* vmtholder, int num_fns, int fn_index, void* fn);

	// Does the casting for us
	template <class T, typename R, typename... Args>
	static long* hook(T* obj, long* vmtholder, int num_fns, int fn_index, R (*fn)(Args...))
	{
		return hook((void*)obj, vmtholder, num_fns, fn_index, (void*)fn);
	}

	vthook(void* tobj, int num_fns, int fn_index, void* fn)
	: newvmt(new long[num_fns]),
		origvmt(hook(tobj, newvmt, num_fns, fn_index, fn)),
		obj(tobj)
	{}

	template <class T, typename R, typename... Args>
	vthook(T* obj, int num_fns, int fn_index, R (*fn)(Args...))
	: vthook((void*)obj, num_fns, fn_index, (void*)fn)
	{}

	~vthook()
	{
		delete[] newvmt;
	}

	long* newvmt;
	long* origvmt;
	void* obj;

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
