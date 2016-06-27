/*
 *  AppWindowBase.cpp
 *  AppTextWindowPlay
 *
 *  Created by David Kieras on 11/9/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */

#include "AppWindowBase.h"
#include "AppClassesUtilities.h"

AppWindowBase::AppWindowBase(CFStringRef main_nibname, CFStringRef window_nibname, CFStringRef window_title) :
	WindowEventUPP(0)
{
	IBNibRef nibRef;
	OSStatus err = CreateNibReference(main_nibname, &nibRef);
	if(err) 
		throw AppClassesCreationError("Could not find supplied nib for window", err);
    err = CreateWindowFromNib(nibRef, window_nibname, &window);
	if(err) 
		throw AppClassesCreationError("Could not create window from supplied nib", err);
//	setup(CFSTR("AppClasses"), window_nibname);
	if(window_title)
		SetWindowTitleWithCFString(window, window_title);
	DisposeNibReference(nibRef);
}

// all cleanup for the window should be done here; application is responsible
// for deleting this object and disposing of the window after handling the
// window-close event.
AppWindowBase::~AppWindowBase()
{
	DisposeEventHandlerUPP(WindowEventUPP);
	DisposeWindow(window);
}

// ptr will contain a pointer to this object
void AppWindowBase::initialize(AppWindowBase * derived_pointer)
{
	// install the handler for window events
	WindowEventUPP = NewEventHandlerUPP(AppWindowBase::AppWindowBaseEventHandler);
	const EventTypeSpec WindowEventsHandled[] = {
		{kEventClassWindow, kEventWindowClose}
		};
	InstallWindowEventHandler(window, WindowEventUPP,
		GetEventTypeCount(WindowEventsHandled), WindowEventsHandled, derived_pointer, NULL);
		    }

void AppWindowBase::show()
{
    ShowWindow( window );
}

void AppWindowBase::move_to(int x, int y)		// move the window to the x, y global coordinates
{
	MoveWindowStructure(window, x, y);
}


// the static event handler function - all specified events related to this window should come here
OSStatus AppWindowBase::AppWindowBaseEventHandler(EventHandlerCallRef inCallRef, EventRef inEvent, void *userData)
{
	OSStatus retVal = eventNotHandledErr;
	AppWindowBase * derived_pointer = static_cast<AppWindowBase *>(userData);
	
	switch(GetEventClass(inEvent)) {
		case kEventClassControl: {
			retVal = derived_pointer->HandleControlEvent(inCallRef, inEvent);
			} 
			break;
		case kEventClassWindow: {
			retVal = derived_pointer->HandleWindowEvent(inCallRef, inEvent);
			}
			break;
		case kEventClassCommand: {
			retVal = derived_pointer->HandleCommandEvent(inCallRef, inEvent);
			}
			break;
	}
	
	return retVal;
}

// this should now be in a member function context
OSStatus AppWindowBase::HandleWindowEvent(EventHandlerCallRef inCallRef, EventRef inEvent)
{
	OSStatus retVal = eventNotHandledErr;
	
	switch(GetEventKind(inEvent)) {
		case kEventWindowClose: {
			retVal = send_window_close_event();
			}
			break;
		}

	return retVal;
}


// Send the Application object an event to have it dispose of this AppWindowBase object
// and remove it from the container of windows.
OSStatus AppWindowBase::send_window_close_event()
{
	EventRef theEvent;
	OSStatus err;
	CreateEvent(NULL, kEventClassWindow, kEventWindowClose, 0, 0, &theEvent);
	SetEventParameter(theEvent, kEventParamDirectObject, typeWindowRef, sizeof(WindowRef), &window);
	err = SendEventToEventTarget(theEvent, GetApplicationEventTarget());
	ReleaseEvent(theEvent);
	return err;
}

// this should now be in a member function context
OSStatus AppWindowBase::HandleCommandEvent(EventHandlerCallRef inCallRef, EventRef inEvent)
{
	OSStatus retVal = eventNotHandledErr;
	return retVal;
}

// this should now be in a member function context
OSStatus AppWindowBase::HandleControlEvent(EventHandlerCallRef inCallRef, EventRef inEvent)
{
	OSStatus retVal = eventNotHandledErr;
	return retVal;
}

