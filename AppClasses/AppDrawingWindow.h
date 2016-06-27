/*
 *  AppWindow.h
 *
 *  Created by David Kieras on 10/22/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */
 
 #ifndef APPDRAWINGWINDOW_H
 #define APPDRAWINGWINDOW_H
 #include <Carbon/Carbon.h>
 #include "AppWindowBase.h"

 /*
 A shallow wrapper around a window that contains a single HIView used for drawing.
 It computes the view size when we draw the content, and moves the origin to lower-left
 and inverts the y-axis, sets default black stroke and fill, but otherwise does nothing
 to the drawing state or metrics.
 */

class AppDrawingWindow : public AppWindowBase {
public:
	AppDrawingWindow(CFStringRef window_title);
	virtual ~AppDrawingWindow();
	
	void set_changed() 
		{changed = true;}
	// mark the HIView for redisplay if changed_flag is on
	// Window events cause a redraw regardless
	virtual void refresh();
			
	CGSize get_view_size() const
		{return view_size;}
			
protected:
	// called from the derived class constructor to provide the appropriate "this" pointer.
	void initialize(AppDrawingWindow * derived_pointer);
	// called by the static event handler function in AppWindowBase
	virtual OSStatus HandleControlEvent(EventHandlerCallRef inCallRef, EventRef inEvent);
	
	// Called to draw the content - must be overridden
	virtual void draw_content(CGContextRef ctx) = 0;

private:
	HIViewRef view_ref;							// the associated HIView
	EventHandlerUPP ControlEventUPP;			// the associated UPP
	CGSize view_size;							// calculated whenever the window-redraw event is handled
	bool changed;							// set by application code to enable refresh
};

#endif


