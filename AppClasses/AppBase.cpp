/*
 *  AppBase.cpp
 *  AppTextWindowPlay
 *
 *  Created by David Kieras on 11/9/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */

#include "AppBase.h"
#include "AppWindowBase.h"
#include "AppClassesUtilities.h"
#include <cassert>
//#include <iostream>

//using std::cout;	using std::cerr;	using std::endl;
using std::map;	using std::make_pair;
//using std::assert;

AppBase::AppBase(CFStringRef main_nibname) :
	WindowEventUPP(0), MenuEventUPP(0)
{
	IBNibRef nibRef;
	OSStatus err;
	// Create a Nib reference passing the name of the nib file (without the .nib extension)
	// CreateNibReference only searches into the application bundle.
	err = CreateNibReference(main_nibname, &nibRef);
	if(err)
		throw AppClassesCreationError("Could not find main nib", err);
	// Once the nib reference is created, set the menu bar. "MainMenu" is the name of the menu bar
	// object. This name is set in InterfaceBuilder when the nib is created.
	err = SetMenuBarFromNib(nibRef, CFSTR("MenuBar"));
	if(err)
		throw AppClassesCreationError("Could not set menu bar from nib", err);
	// We don't need the nib reference anymore.
	DisposeNibReference(nibRef);
}

AppBase::~AppBase()
{
//	cout << "AppBase dtor" << endl;
	// dispose of any unclosed windows
	for(Window_map_t::iterator it = window_map.begin(); it != window_map.end(); ++it)
		delete (it->second);
	
	DisposeEventHandlerUPP(WindowEventUPP);
	DisposeEventHandlerUPP(MenuEventUPP);
}

void AppBase::initialize(AppBase * derived_ptr)
{
	// install an app-level event handler to accept window-close events
	WindowEventUPP = NewEventHandlerUPP(AppBase::WindowEventHandler);
	const EventTypeSpec WindowEventsHandled[] = {
//		{'XXXX', 'YYYY'}
		{kEventClassWindow, kEventWindowClose}
		};
	InstallApplicationEventHandler(WindowEventUPP,
		GetEventTypeCount(WindowEventsHandled), WindowEventsHandled, (void *)derived_ptr, NULL);
		
// install a handler for menu events
	EventTypeSpec cmdEvents[] = { {kEventClassCommand, kEventProcessCommand}, {kEventClassCommand, kEventCommandUpdateStatus} };
	MenuEventUPP = NewEventHandlerUPP(AppBase::MenuCommandEventHandler);
	InstallApplicationEventHandler(MenuEventUPP, GetEventTypeCount(cmdEvents), cmdEvents, (void *)derived_ptr, NULL);
}



void AppBase::add_window(AppWindowBase * p)
{
	window_map.insert(make_pair(p->get_WindowRef(), p));
}

bool AppBase::window_present(AppWindowBase * p)
{
	return window_present(p->get_WindowRef());
}

bool AppBase::window_present(WindowRef theWindow)
{
	Window_map_t::iterator it = window_map.find(theWindow);
	return (it != window_map.end());
}
	

void AppBase::close_window(AppWindowBase * p)
{
	close_window(p->get_WindowRef());
}

void AppBase::close_window(WindowRef theWindow)
{
	Window_map_t::iterator it = window_map.find(theWindow);
	if(it == window_map.end()) {
		assert(!"Trying to close an unknown window");
//		cerr << "Trying to close an unknown window" << endl;
		return;
		}
	// call a possible derived-class function to act on the window closing
	window_closing(it->second);
		
	delete (it->second);
	window_map.erase(it);
}

AppWindowBase * AppBase::get_window_ptr(WindowRef theWindow)
{
	Window_map_t::iterator it = window_map.find(theWindow);
	if(it == window_map.end()) {
		assert(!"Trying to get an unknown window");
//		cerr << "Trying to close an unknown window" << endl;
		return 0;
		}
	return it->second;
}


/* This handles the window-close events in order to maintain the container of open windows */
OSStatus AppBase::WindowEventHandler(EventHandlerCallRef inCallRef, EventRef inEvent, void *userData)
{
	OSStatus retVal = eventNotHandledErr;
	
	AppBase * derived_ptr = static_cast<AppBase *>(userData);
	
	switch(GetEventClass(inEvent)) {
		case kEventClassWindow: {
			switch(GetEventKind(inEvent)) {
				case kEventWindowClose: {
					WindowRef window;					
					GetEventParameter(inEvent, kEventParamDirectObject, typeWindowRef, NULL, sizeof(WindowRef), NULL, &window);
					derived_ptr->close_window(window);
					retVal = noErr;
					}
				break;
				}
			}
			break;
		case kEventClassCommand: {
			retVal = noErr;
			}
			break;
	}
	
	return retVal;
}

OSStatus AppBase::MenuCommandEventHandler(EventHandlerCallRef handlerRef, EventRef event, void *userData)
{
	OSStatus result = eventNotHandledErr;
	AppBase * derived_ptr = static_cast<AppBase *>(userData);

	HICommand command;
	
	GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &command);
	
	switch (GetEventKind(event)) {
		case kEventCommandUpdateStatus: {
			result = derived_ptr->HandleMenuUpdateStatus(command);
			break;
			}
		case kEventCommandProcess: {
			result = derived_ptr->HandleMenuProcessCommand(command);
			break;
			}
		}
		
	return result;
}


OSStatus AppBase::HandleMenuUpdateStatus(HICommand command)
{
	OSStatus result = eventNotHandledErr;

/*	
	switch (command.commandID) {
		case kMyzapCommand:
			if(!PlayApp::get_instance().get_optionA()) {
				DisableMenuCommand( NULL, kMyzapCommand );
				}
			else if(PlayApp::get_instance().get_optionA()) {
				EnableMenuCommand( NULL, kMyzapCommand );
				}
			break;
		case kMyfranCommand:
			if(!PlayApp::get_instance().get_optionA()) {
				EnableMenuCommand( NULL, kMyfranCommand );
				}
			else if(PlayApp::get_instance().get_optionA()) {
				DisableMenuCommand( NULL, kMyfranCommand );
				}
			break;
			}
	break;
	}
*/
	return result;
}


OSStatus AppBase::HandleMenuProcessCommand(HICommand command)
{
	OSStatus result = eventNotHandledErr;

/*	switch(command.commandID) {
		case kMyzapCommand:
			SysBeep(2);
			g_optionA = 0;
//			HIViewSetNeedsDisplay(g_flagView, true);
			result = noErr;
			break;
		case kMyfranCommand:
			g_optionA = 1;
//			HIViewSetNeedsDisplay(g_flagView, true);
			result = noErr;
			break;
		case kMydialogCommand:
//			result = ShowMyDialog();
			result = noErr;
			break;
		case kMyReadCommand:
//			g_file_read_path.clear();
//			result = ShowMyNavOpenDialog();
			// open and read the file
//			read_my_file();
			result = noErr;
			break;
		case kMySpewCommand:
//			result = ShowMyNavSaveAsDialog();
//			write_my_file();
			result = noErr;
			break;
		}
*/		
	return result;
}

