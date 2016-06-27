/*
 *  AppBase.h
 *
 *  Created by David Kieras on 11/9/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */

#ifndef APPBASE_H
#define APPBASE_H
#include <Carbon/Carbon.h>
#include <map>

class AppWindowBase;

class AppBase {
public:
	AppBase(CFStringRef main_nibname = CFSTR("main"));
	virtual ~AppBase();
	// add a window to the container for closing
	void add_window(AppWindowBase * p);
	bool window_present(AppWindowBase * p);
	bool window_present(WindowRef theWindow);
	void close_window(AppWindowBase * p);
	void close_window(WindowRef theWindow);
	// return a pointer to the corresponding AppWindow object
	AppWindowBase * get_window_ptr(WindowRef theWindow);

protected:
	// call from derived class constructor
	void initialize(AppBase * derived_ptr);

	// this event handler will accept menu events
	static OSStatus MenuCommandEventHandler(EventHandlerCallRef inCallRef, EventRef inEvent, void *userData);
	// the handlers for menu commands in this class do nothing and would normally be overridden
	virtual OSStatus HandleMenuUpdateStatus(HICommand command);
	virtual OSStatus HandleMenuProcessCommand(HICommand command);
	
	// this function is called when a window close event is acted on;
	// derived classes should override in order to do any additional window-closing maintenance,
	// such as removing the window from a list of views, etc.
	virtual void window_closing(AppWindowBase * p) {}

	// this event handler accepts close events, which come with a WindowRef, and
	// calls AppBase's window-closing function to ensure that both the Carbon window and
	// the AppWindow object are disposed of simultaneously.
	static OSStatus WindowEventHandler(EventHandlerCallRef inCallRef, EventRef inEvent, void *userData);
	
private:
	/* a map of WindowRef to AppWindowBase pointer is required because I can't figure out how
	to pass a pointer to a AppWindowBase object through the Carbon Event callback.
	Whenever a AppWindowBase is created, it needs to be added to the map with the add_window function. */
	typedef std::map<WindowRef, AppWindowBase *> Window_map_t;
	Window_map_t window_map;
	EventHandlerUPP WindowEventUPP;
	EventHandlerUPP MenuEventUPP;
};

#endif
