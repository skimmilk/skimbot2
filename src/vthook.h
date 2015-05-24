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

// Multihook supports hooking multiple methods, but with some more effort
// First template argument is the number of hooks we are going to hook in the class
// Second template argument are the maximum number of virtual functions in the class
template <int num_hooks, int num_virtfns>
class multihook
{
private:
	long* origvmt;
	void* obj;
	long newvmt[num_virtfns];

	struct hookinfo { long fnptr; int position; };
	hookinfo hookfns[num_hooks];

public:
	~multihook()
	{
		unhook_all();
	}

	template <typename T>
	inline void init(T* o)
	{
		// Save the original values
		obj = (void*)o;
		origvmt = *((long**)o);

		// Copy the vmt
		for (int i = 0; i < num_virtfns; i++)
			newvmt[i] = origvmt[i];

		// Set the object's vmt to the newvmt
		long** vmtptr = (long**)obj;
		*vmtptr = newvmt;
	}

	inline void unhook_all()
	{
		// Restore vmt pointer to original
		long** vmtptr = (long**)obj;
		*vmtptr = origvmt;
	}

	template <typename Ret, typename... Args>
	inline void init_hook(int index, Ret (*fn)(Args...), int vmtpos)
	{
		hookfns[index] = {(long)fn, vmtpos};
	}
	inline void hook(int index)
	{
		newvmt[hookfns[index].position] = hookfns[index].fnptr;
	}
	inline void unhook(int index)
	{
		int pos = hookfns[index].position;
		newvmt[pos] = origvmt[pos];
	}
};

} /* namespace skim */

#endif /* VTHOOK_H_ */
