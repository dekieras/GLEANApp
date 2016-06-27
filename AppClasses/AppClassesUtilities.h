/*
 *  AppClassesUtilities.h
 *
 *  Created by David Kieras on 11/9/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */
#ifndef APPCLASSESUTILITIES_H
#define APPCLASSESUTILITIES_H

#include <Carbon/Carbon.h>
#include <string>

// an exception class - don't throw exceptions from code that was called
// from an event handler ...
struct AppClassesCreationError {
	AppClassesCreationError(const char * msg_, OSStatus err_) : msg(msg_), err(err_) {}
	const char * msg;
	OSStatus err;
};


// the result of this function needs to be released at some point
// with CFRelease(the_CFStrinRef);
CFStringRef convert_to_CFString(const char * c_str);
CFStringRef convert_to_CFString(const std::string& str);

std::string convert_to_std_string(CFStringRef cf_str);

#endif

