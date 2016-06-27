/*
 *  BreakDialog.cpp
 *  EPICX
 *
 *  Created by David Kieras on 2/2/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "BreakDialog.h"
#include "AppClassesUtilities.h"
#include "GleanApp.h"
#include "GLEANKernel/Model.h"
#include <cassert>

using std::string;	using std::list;

static const ControlID kBdia_all_breaks_enabled = {'Bdia', 101};
static const ControlID kBdia_rule_break_browser = {'Bdia', 110};
enum {kMyEnabColumn = 'ENAB', kMyRuleNameColumn = 'RNAM'};	// does this need to be an ENUM?

// hopefully! temporary globals
BreakDialog * BreakDialog::dialog_ptr = 0;
//bool BreakDialog::enabled = false;
//std::vector<Symbol> BreakDialog::names;
//std::vector<bool> BreakDialog::org_states;
//std::vector<bool> BreakDialog::states;

BreakDialog::BreakDialog() :
	AppDialogBase(CFSTR("GLEANApp"), CFSTR("BreakDialog"))
{
	AppDialogBase::initialize_dialog(this);
}

// install the data browser callbacks and populate the browser data
void BreakDialog::set_initial_dialog_data()
{
	Model * model_ptr = GleanApp::get_instance().get_model_ptr();
	set_checkbox_value(kBdia_all_breaks_enabled, model_ptr->get_breaks_enabled());

	OSStatus status = noErr;
	WindowRef window = get_WindowRef();
//	DataBrowserCallbacks  dbCallbacks;
//	status = CreateWindowFromNib(nibRef, CFSTR("BreakList"), &window);
	HIViewFindByID(HIViewGetRoot(window), kBdia_rule_break_browser, &breaklistview);
	dbCallbacks.version = kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks (&dbCallbacks);
	dbCallbacks.u.v1.itemDataCallback = 
            NewDataBrowserItemDataUPP((DataBrowserItemDataProcPtr)BreakDialog::AppBrowserItemDataCallback);
    SetDataBrowserCallbacks(breaklistview, &dbCallbacks);						 	
    SetAutomaticControlDragTrackingEnabledForWindow (window, true);
	
	//kDataBrowserPropertyIsEditable
	//kDataBrowserCheckboxType
	
	// set up the data for the callback to munge
//	names.clear();
//	org_states.clear();
//	states.clear();
	const list<string>& rule_names = model_ptr->get_step_names();
	for(list<string>::const_iterator it = rule_names.begin(); it != rule_names.end(); ++it) {
		string rule_name = *it;
		names.push_back(rule_name);
		bool state = model_ptr->get_break_state(rule_name);
		org_states.push_back(state);
		states.push_back(state);
		}
		
	dialog_ptr = this;
	int numRows = names.size();
    status = AddDataBrowserItems (breaklistview, kDataBrowserNoItem, numRows, 
                            NULL, kDataBrowserItemNoProperty );
}


void BreakDialog::read_dialog_data()
{
	Model * model_ptr = GleanApp::get_instance().get_model_ptr();
	model_ptr->set_breaks_enabled(get_checkbox_value(kBdia_all_breaks_enabled));
	int n = names.size();
	for(int i = 0; i < n; ++i) {
		if(org_states[i] != states[i]) 
			model_ptr->set_break_state(names[i], states[i]);
		}
//	// try to be at least slightly neat
//	names.clear();
//	org_states.clear();
//	states.clear();
}


OSStatus    BreakDialog::AppBrowserItemDataCallback (ControlRef browser, 
                DataBrowserItemID itemID, 
                DataBrowserPropertyID property, 
                DataBrowserItemDataRef itemData, 
                Boolean changeValue)
{  
	// WindowRef window = GetControlOwner(browser); // the App object doesn't know this window yet
	// AppWindowBase * awbp = GleanApp::get_instance().get_window_ptr(window);	// so this fails
	// BreakDialog * bdp = static_cast<BreakDialog *>(awbp);
	// assert(bdp == dialog_ptr);
	
	BreakDialog * dialog_ptr = BreakDialog::dialog_ptr;
	
	OSStatus status = noErr;
 
    if (!changeValue) switch (property)
    {
        case kMyEnabColumn: {
			int i = itemID - 1;
			int v = dialog_ptr->states[i];
			if(v) 
				status = SetDataBrowserItemDataButtonValue(itemData, kThemeButtonOn);
			else
				status = SetDataBrowserItemDataButtonValue(itemData, kThemeButtonOff);
			}
            break;        
        case kMyRuleNameColumn: {
			int i = itemID - 1;
//			CFStringRef text = CFStringCreateWithFormat(NULL, NULL, CFSTR("Rule Number %d"), i);
			const char * text_ptr = dialog_ptr->names[i].c_str();
			CFStringRef text = convert_to_CFString(text_ptr);
            status = SetDataBrowserItemDataText(itemData, text);
			}
            break;
        default:
            status = errDataBrowserPropertyNotSupported;
            break;
    }
	else if(changeValue) switch (property)
    {
        case kMyEnabColumn: {
			int i = itemID - 1;
			ThemeButtonValue  v;
			status = GetDataBrowserItemDataButtonValue(itemData, &v);
			
			if(v == kThemeButtonOn) 
				dialog_ptr->states[i] = 1;
			else if(v == kThemeButtonOff)
				dialog_ptr->states[i] = 0;
			}
            break;        
        case kMyRuleNameColumn:
//            status = SetDataBrowserItemDataText(itemData,
//                    CFSTR("This is a rule name"));
            break;
        default:
            status = errDataBrowserPropertyNotSupported;
            break;
    }
	
    else status = errDataBrowserPropertyNotSupported; // 3
 
 return status;
}

