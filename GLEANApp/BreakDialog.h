/*
 *  BreakDialog.h
 *  EPICX
 *
 *  Created by David Kieras on 2/2/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef BREAKDIALOG_H
#define BREAKDIALOG_H
#include <Carbon/Carbon.h>
#include "AppDialogBase.h"
#include "GLEANKernel/Symbol.h"
#include <string>
#include <vector>

class BreakDialog : public AppDialogBase {
public:
	BreakDialog();

protected:
	virtual void set_initial_dialog_data();
	virtual void read_dialog_data();
	
private:
	DataBrowserCallbacks  dbCallbacks;
	HIViewRef breaklistview;
	bool enabled;
	std::vector<std::string> names;
	std::vector<bool> org_states;
	std::vector<bool> states;
	
	// UGLY UGLY UGLY - but I don't know how to get a pointer to this object
	// into the call back yet
	static BreakDialog * dialog_ptr;

	// in for a penny, in for a pound
	friend OSStatus AppBrowserItemDataCallback(ControlRef browser, 
                DataBrowserItemID itemID, DataBrowserPropertyID property, 
                DataBrowserItemDataRef itemData, Boolean changeValue);
	
	// the static data browser callback handler
	static OSStatus AppBrowserItemDataCallback (ControlRef browser, 
                DataBrowserItemID itemID, DataBrowserPropertyID property, 
                DataBrowserItemDataRef itemData, Boolean changeValue);
};
#endif

