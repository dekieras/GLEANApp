/* This defines a generic device that provides for all of the operators defined in GOMSL
and provides some basic facilities for interacting with the simulated human. The default
behaviors defined in this device simply echo the supplied input and generate no output.
The output functions are provided as an inheritable facility.
*/

#ifndef DUMMY_DEVICE_H
#define DUMMY_DEVICE_H

#include "GLEANKernel/Device_base.h"

class Dummy_device : public Device_base {
public:
	Dummy_device(Output_tee& ot) :
		Device_base(std::string("Dummy_device"), ot)
		{}
	
	virtual void initialize();	// to set trace flag
	// override none of the other functions in Device_base
};

#endif
