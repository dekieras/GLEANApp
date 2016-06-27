/*
 *  TextView.h
 *  EPICApp
 *
 *  Created by David Kieras on 1/14/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include <Carbon/Carbon.h>
#include "AppTextWindow.h"
#include "GLEANKernel/View_base.h"

#include <string>

class TextView : public AppTextWindow, public View_base {
public:
	TextView(CFStringRef window_title);
		
	// overrides for View_base
	virtual void clear()
		{clear_text();}
	virtual void notify_append_text(const std::string& str)
		{append_text(str.c_str());}
};

#endif
