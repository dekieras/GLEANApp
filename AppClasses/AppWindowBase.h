/*
 *  AppWindowBase.h
 *
 *  Created by David Kieras on 11/9/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */
#ifndef APPWINDOWBASE_H
#define APPWINDOWBASE_H
#include <Carbon/Carbon.h>

 /*
 A shallow wrapper around a window with the minimal event handling.
 Window close events are intercepted and sent to the main application
 event handler to allow for the coordinated maintainance of window objects.
 */

class AppWindowBase {
public:
	AppWindowBase(CFStringRef main_nibname, CFStringRef window_nibname, CFStringRef window_title = 0);
//	AppWindowBase(IBNibRef nibRef, CFStringRef window_nibname, CFStringRef window_title);
	virtual ~AppWindowBase();
	
	WindowRef get_WindowRef() const {return window;}
	
	// unclear what the general implementation is, so none in this class
	virtual void refresh() {}
	
	void move_to(int x, int y);		// move the window to the x, y global coordinates

	void show();					// show the window - call after setting all parameters;
		
protected:
	// called from the derived class constructors to provide the appropriate "this" pointer.
	void initialize(AppWindowBase * derived_pointer);
	
	// the static event handler function - all events are routed to this handler and dispatched to specific
	// handlers for the event class - protected to allow derived classes to install it for other events
	static OSStatus AppWindowBaseEventHandler(EventHandlerCallRef inCallRef, EventRef inEvent, void *userData);
	
	// called by AppWindowBaseEventHandler for the specific window object involved.
	// in this class, the WindowEvent handler routes the window-close event to the application
	// if overridden this function should still be called to do handle the window-close event
	virtual OSStatus HandleWindowEvent(EventHandlerCallRef inCallRef, EventRef inEvent);
	// derived classes can also call this function if they don't automatically generate a window-close event
	OSStatus send_window_close_event();
	// the handlers for Command Events and Control Events are empty in this class, returning event-not-handled.
	virtual OSStatus HandleCommandEvent(EventHandlerCallRef inCallRef, EventRef inEvent);
	virtual OSStatus HandleControlEvent(EventHandlerCallRef inCallRef, EventRef inEvent);

private:
	WindowRef window;						// the associated window
	EventHandlerUPP WindowEventUPP;			// the associated UPP
};

#endif
