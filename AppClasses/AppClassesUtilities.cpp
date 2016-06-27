#include "AppClassesUtilities.h"

using std::string;

// the result of this function needs to be released at some point
// with CFRelease(the_CFStrinRef);
CFStringRef convert_to_CFString(const char * c_str)
{
	CFStringEncoding encoding = kCFStringEncodingMacRoman;
	CFAllocatorRef alloc_default = kCFAllocatorDefault;
	return CFStringCreateWithCString(alloc_default, c_str, encoding);
}

// the result of this function needs to be released at some point
// with CFRelease(the_CFStrinRef);
CFStringRef convert_to_CFString(const std::string& std_str)
{
	CFStringEncoding encoding = kCFStringEncodingMacRoman;
	CFAllocatorRef alloc_default = kCFAllocatorDefault;
	return CFStringCreateWithCString(alloc_default, std_str.c_str(), encoding);
}


string convert_to_std_string(CFStringRef cf_str)
{
	CFStringEncoding encoding = kCFStringEncodingMacRoman;
//	CFStringEncoding encoding = kCFStringEncodingUTF8;
	
	// see if we can do it quickly
	const char * cstr_ptr = CFStringGetCStringPtr(cf_str, encoding);
	if(cstr_ptr)
		return string(cstr_ptr);

	// do it slowly
	// get length, adding 1 because of uncertainty on whether the terminating null by is included
	CFIndex buf_len = 
		CFStringGetMaximumSizeForEncoding(CFStringGetLength(cf_str), encoding) + 1;	
	char * buffer = new char[buf_len];
	Boolean result = CFStringGetCString(cf_str, buffer, buf_len, encoding);
	if(!result)
		throw AppClassesCreationError("Failure to convert CFString to std::string", 0);
	
	std::string str_result(buffer);
	delete[] buffer;
	
	return str_result;
}

