/*
 *  AppDrawingWindow.cpp
 *
 *  Created by David Kieras on 10/22/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */

#include "AppDrawingWindow.h"
#include "AppClassesUtilities.h"
#include <cassert>

static const ControlID kViewControlID = { 'ApDW', 0 };

AppDrawingWindow::AppDrawingWindow(CFStringRef window_title) :
	AppWindowBase(CFSTR("AppClasses"), CFSTR("AppDrawingWindow"), window_title), view_ref(0), 
	ControlEventUPP(0), changed(false)
{
}

AppDrawingWindow::~AppDrawingWindow()
{
	DisposeEventHandlerUPP(ControlEventUPP);
}

// mark the HIView for redisplay if changed_flag is on
void AppDrawingWindow::refresh()
{
	if(view_ref && changed) {
		HIViewSetNeedsDisplay(view_ref, true);
		changed = false;
		}
}

// called from the derived class constructor to provide the appropriate "this" pointer.
void AppDrawingWindow::initialize(AppDrawingWindow * derived_pointer)
{
	OSStatus err;
	// install the handler for the embedded view draw-content event
	ControlEventUPP = NewEventHandlerUPP(AppWindowBase::AppWindowBaseEventHandler);
	const EventTypeSpec ContentEventsHandled[] = {
		{kEventClassControl, kEventControlDraw}
		};
	err = HIViewFindByID(HIViewGetRoot(get_WindowRef()), kViewControlID, &view_ref);
	if(err)
		throw AppClassesCreationError("Could not find embedded view in AppDrawingWindow", err);
	InstallControlEventHandler(view_ref, ControlEventUPP, 
		GetEventTypeCount(ContentEventsHandled), ContentEventsHandled, derived_pointer, NULL);

	// continue initialization of the base class which will show the window
	AppWindowBase::initialize(derived_pointer);
}

// called in a member function context
OSStatus AppDrawingWindow::HandleControlEvent(EventHandlerCallRef inCallRef, EventRef inEvent)
{
	OSStatus retVal = eventNotHandledErr;
	
	switch(GetEventKind(inEvent)) {
		case kEventControlDraw : 
			HIViewRef the_view;
			HIRect view_bounds;
			CGContextRef ctx;

			// extract the parameters we need to draw the view
			GetEventParameter(inEvent, kEventParamDirectObject, typeControlRef, NULL, sizeof(the_view), NULL, &the_view);
			assert(the_view == view_ref);

			GetEventParameter(inEvent, kEventParamCGContextRef, typeCGContextRef, NULL, sizeof(ctx), NULL, &ctx);
			// Reorient the coordinate system so that the origin is in the lower left corner of 
			// the view and the Y axis points "up" on the window.
			HIViewGetBounds(the_view, &view_bounds);
			view_size = view_bounds.size;

			// draw a very light gray background
//			CGContextSetRGBFillColor(ctx, 0.9, 0.9, 0.9, 1.0);
//			CGContextFillRect(ctx, view_bounds);
			
			// set the stroke and fill color to some defaults
			CGContextSetRGBFillColor(ctx, 0., 0., 0., 1.0);
			CGContextSetRGBStrokeColor(ctx, 0., 0., 0., 1.0);
			
			CGContextTranslateCTM(ctx, 0, view_size.height);	// move the origin to the lower left
			CGContextScaleCTM(ctx, 1.0, -1.0);					// flip the Y-axis
						
			// call the specific derived class function
			draw_content(ctx);
		
			retVal = noErr;
		break;
	}
	
	return retVal;
}

// This testing function would normally be overridden
void AppDrawingWindow::draw_content(CGContextRef ctx)
{
			// write a message just to show base at work
			char msg[50]  = "I'm AppDrawingWindow!";
			// write a bit of text
			CGContextSelectFont(ctx, "Geneva", 12, kCGEncodingMacRoman);
			// second param must be length
			CGContextShowTextAtPoint(ctx, 0, 0, msg, strlen(msg));
//			CGContextShowText(ctx, &s[1], s[0]);
			
}

