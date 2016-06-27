/*
 *  MemoryView.h
 *
 *  Created by David Kieras on 033108.
 *  Copyright 2008 University of Michigan. All rights reserved.
 *
 */

#ifndef MEMORYVIEW_H
#define MEMORYVIEW_H
#include <Carbon/Carbon.h>
#include "AppDrawingWindow.h"
#include "GLEANKernel/View_base.h"	// includes Geometry Utilities

// a pure interface class

class MemoryView : public AppDrawingWindow, public View_base {
public:
	MemoryView(CFStringRef window_name) : AppDrawingWindow(window_name) {}

	virtual ~MemoryView() {}

	// called from base class to redraw the window
	virtual void draw_content(CGContextRef ctx) = 0;
};

#endif
