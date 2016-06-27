/*
 *  AppDialogBase.cpp
 *  AppClassesDev
 *
 *  Created by David Kieras on 11/10/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */

#include "AppDialogBase.h"
#include "AppClassesUtilities.h"
//#include <iostream>

//using std::cout;	using std::endl;
using std::string;

AppDialogBase::AppDialogBase(CFStringRef main_nibname, CFStringRef dialog_nibname) :
	AppWindowBase(main_nibname, dialog_nibname), DialogEventUPP(0)
{
}

// all cleanup for the dialog should be done here; application is responsible
// for deleting this object and disposing of the window after handling the
// window-close event.
AppDialogBase::~AppDialogBase()
{
//	cout << "AppDialogBase dtor" << endl;
	DisposeEventHandlerUPP(DialogEventUPP);
}

// ptr will contain a pointer to this object
// do not initialize AppWindowBase - event handler is different
void AppDialogBase::initialize_dialog(AppDialogBase * derived_pointer)
{
	WindowRef window = get_WindowRef();
	// install the handler for dialog command events
	DialogEventUPP = NewEventHandlerUPP(AppDialogBase::AppDialogBaseEventHandler);
	const EventTypeSpec WindowEventsHandled[] = {
		{kEventClassCommand, kEventProcessCommand}
 		};
	InstallWindowEventHandler(window, DialogEventUPP,
		GetEventTypeCount(WindowEventsHandled), WindowEventsHandled, derived_pointer, NULL);
		
	set_initial_dialog_data();

    ShowWindow(window);
}

// the static event handler function - all specified events related to this window should come here
OSStatus AppDialogBase::AppDialogBaseEventHandler(EventHandlerCallRef inCallRef, EventRef inEvent, void *userData)
{
	OSStatus result = eventNotHandledErr;
	AppDialogBase * derived_pointer = static_cast<AppDialogBase *>(userData);
	HICommand command;
		
	GetEventParameter(inEvent, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &command);

	switch(command.commandID) {
		case kHICommandOK: {
			derived_pointer->read_dialog_data();		
			// tell the application we are done with the window
			derived_pointer->send_window_close_event();
			result = noErr;
			}
			break;
		case kHICommandCancel:
			derived_pointer->send_window_close_event();
			result = noErr;
			break;
		default:
			result = derived_pointer->handle_command(command);
			break;
		}
	return result;
}

OSStatus AppDialogBase::handle_command(HICommand command)
{
	OSStatus result = eventNotHandledErr;
	return result;
}

void AppDialogBase::set_checkbox_value(HIViewID checkbox_id, bool state) 
{
	HIViewRef controlview;
	if(!HIViewFindByID(HIViewGetRoot(get_WindowRef()), checkbox_id, &controlview))
		SetControl32BitValue(controlview, state);
}

bool AppDialogBase::get_checkbox_value(HIViewID checkbox_id) 
{
	HIViewRef controlview;
	if(!HIViewFindByID(HIViewGetRoot(get_WindowRef()), checkbox_id, &controlview))
		return bool(GetControl32BitValue(controlview));
	return false;
}

// set the button number in a radio button group
void AppDialogBase::set_radiogroup_number(HIViewID radiogroup_id, int button_number) 
{
	HIViewRef controlview;
	if(!HIViewFindByID(HIViewGetRoot(get_WindowRef()), radiogroup_id, &controlview))
		SetControl32BitValue(controlview, button_number);
}

int AppDialogBase::get_radiogroup_number(HIViewID radiogroup_id) 
{
	HIViewRef controlview;
	if(!HIViewFindByID(HIViewGetRoot(get_WindowRef()), radiogroup_id, &controlview))
		return int(GetControl32BitValue(controlview));
	return false;
}




void AppDialogBase::set_button_label(HIViewID button_id, CFStringRef in_string) 
{
	HIViewRef controlview;
	if(!HIViewFindByID(HIViewGetRoot(get_WindowRef()), button_id, &controlview))
		SetControlTitleWithCFString(controlview, in_string);
}

void AppDialogBase::set_CFString_value(HIViewID stringbox_id, CFStringRef in_string) 
{
	HIViewRef controlview;
	if(!HIViewFindByID(HIViewGetRoot(get_WindowRef()), stringbox_id, &controlview))
		SetControlData(controlview, kControlEntireControl, kControlEditTextCFStringTag, sizeof(CFStringRef), &in_string);
}

void AppDialogBase::set_std_string_value(HIViewID stringbox_id, const string& in_string) 
{
	CFStringRef cf_str = convert_to_CFString(in_string);
	set_CFString_value(stringbox_id, cf_str);
	CFRelease(cf_str);
}

CFMutableStringRef AppDialogBase::get_CFString_value(HIViewID stringbox_id)
{
	HIViewRef controlview;
	CFMutableStringRef string_value = 0;
	if(!HIViewFindByID(HIViewGetRoot(get_WindowRef()), stringbox_id, &controlview))
		GetControlData(controlview, kControlEntireControl, kControlEditTextCFStringTag, sizeof(CFMutableStringRef), &string_value, NULL);
	return string_value;
}

string AppDialogBase::get_std_string_value(HIViewID stringbox_id)
{
	CFMutableStringRef cf_str = get_CFString_value(stringbox_id);
	return convert_to_std_string(cf_str);	
}

void AppDialogBase::set_double_value(HIViewID stringbox_id, CFStringRef format, double value) 
{
	CFStringRef text = CFStringCreateWithFormat(NULL, NULL, format, value);
	HIViewRef controlview;
	if(!HIViewFindByID(HIViewGetRoot(get_WindowRef()), stringbox_id, &controlview))
		SetControlData(controlview, kControlEntireControl, kControlEditTextCFStringTag, sizeof(CFStringRef), &text);
}

double AppDialogBase::get_double_value(HIViewID doublebox_id)
{
	CFStringRef text = get_CFString_value(doublebox_id);
	return CFStringGetDoubleValue(text);
}

void AppDialogBase::set_int_value(HIViewID stringbox_id, int value) 
{
	CFStringRef text = CFStringCreateWithFormat(NULL, NULL, CFSTR("%d"), value);
	HIViewRef controlview;
	if(!HIViewFindByID(HIViewGetRoot(get_WindowRef()), stringbox_id, &controlview))
		SetControlData(controlview, kControlEntireControl, kControlEditTextCFStringTag, sizeof(CFStringRef), &text);
}

double AppDialogBase::get_int_value(HIViewID intbox_id)
{
	CFStringRef text = get_CFString_value(intbox_id);
	return CFStringGetIntValue(text);
}


