//
//  GleanAppmain.cpp
//
//  Created by David Kieras on 11/8/06.
//  Copyright University of Michigan 2006. All rights reserved.
//

#include <Carbon/Carbon.h>
#include "GleanApp.h"
#include "AppClassesUtilities.h"

#include <iostream>

using std::cout;	using std::cerr;	using std::endl;

int main()
{
	OSStatus err = noErr;
	
	try {
	// create the application object - it will receive window close events
	GleanApp::get_instance();
	}
	// I should catch all exceptions here ...
	catch(AppClassesCreationError& x) {
		cerr << x.msg << ' ' << x.err << endl;
		return x.err;
		}	
	
	RunApplicationEventLoop();
			
	return err;
}


