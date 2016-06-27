#include "GLEANKernel/Output_tee_globals.h"
#include "Dummy_device.h"

// for use in non-dynamically loaded models
Device_base * create_Dummy_device()
{
	return new Dummy_device(Normal_out);
}

// the class factory functions to be accessed with dlsym
extern "C" Device_base * create_device() 
{
    return create_Dummy_device();
}

extern "C" void destroy_device(Device_base * p) 
{
    delete p;
}
