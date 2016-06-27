/*
 *  CLGleanStandaloneMain.cpp
 *  CLGlean
 *
 *  Created by David Kieras on 7/26/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "GLEANKernel/Exception.h"
#include "GLEANKernel/Assert_throw.h"
#include "CLGleanApp.h"
//#include "Device_Processor.h"
//#include "Dummy_Device.h"
//#include "GLEANKernel/Output_tee_globals.h"

#include <iostream>
//#include <sstream>
//#include <vector>
//#include <string>

// class Device_Object_Store_Base;

using namespace std;

int main()
{
	try{
		CLGleanApp theApp(0);
		theApp.run_top_level();
		}
	catch(Exception& x) {
		cerr << x.what() << endl;
	}
	catch (Assertion_exception& x) {
		cerr << x.what() << endl;
	}
	catch (...) {
		cerr << "Unknown exception caught" << endl;
	}
   
   return 0;
}

