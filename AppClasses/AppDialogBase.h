/*
 *  AppDialogBase.h
 *  AppClassesDev
 *
 *  Created by David Kieras on 11/10/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */
 
#ifndef APPDIALOGBASE_H
#define APPDIALOGBASE_H
#include "AppWindowBase.h"
#include <string>

  /*
 A shallow wrapper around a window for a dialog, with an
 event handler that dispatches to virtual functions.
 */

class AppDialogBase : public AppWindowBase {
public:
	AppDialogBase(CFStringRef main_nibname, CFStringRef dialog_nibname);
	virtual ~AppDialogBase();
			
protected:
	// called from the derived class constructors to provide the appropriate "this" pointer.
	// creates and displays the dialog
	void initialize_dialog(AppDialogBase * derived_pointer);
	// derived class will override these functions for dialog-specific purposes
	virtual OSStatus handle_command(HICommand command);	// called if command is not OK or cancel
	virtual void set_initial_dialog_data() {}
	virtual void read_dialog_data() {}
	
	// accessors for controls in the dialog - each has a control ID set in IB
	// checkboxes
	void set_checkbox_value(HIViewID checkbox_id, bool state);
	bool get_checkbox_value(HIViewID checkbox_id);
	void set_button_label(HIViewID button_id, CFStringRef in_string);
	void set_radiogroup_number(HIViewID radiogroup_id, int button_number);
	int get_radiogroup_number(HIViewID radiogroup_id);
	
	// CFString fields
//	void set_CFString_value(HIViewID stringbox_id, CFMutableStringRef in_string);
	void set_CFString_value(HIViewID stringbox_id, CFStringRef in_string);
	void set_std_string_value(HIViewID stringbox_id, const std::string& in_string);
	CFMutableStringRef get_CFString_value(HIViewID stringbox_id);
	std::string get_std_string_value(HIViewID stringbox_id);
	// integer fields - integer text written/read to a CFString field
	void set_int_value(HIViewID stringbox_id, int value);
	double get_int_value(HIViewID intbox_id);
	// double fields - double text written/read to a CFString field - format is printf-style
	void set_double_value(HIViewID stringbox_id, CFStringRef format, double value);
	double get_double_value(HIViewID doublebox_id);

private:
	EventHandlerUPP DialogEventUPP;			// the associated UPP

	// the static event handler function - all events are routed to this handler and dispatched to specific
	// handlers for the event class - protected to allow derived classes to install it for other events
	static OSStatus AppDialogBaseEventHandler(EventHandlerCallRef inCallRef, EventRef inEvent, void *userData);
};



#endif
