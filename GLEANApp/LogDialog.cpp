/*
 *  LogDialog.cpp
 *  EPICX
 *
 *  Created by David Kieras on 2/3/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "LogDialog.h"
#include "GleanApp.h"
#include "AppNavDialogs.h"
#include "GLEANKernel/Output_tee_globals.h"
#include <iostream>
#include <fstream>
#include <string>

using std::ostream;	using std::ofstream;
using std::string;

// Display dialog control constants
static const ControlID kLdia_normal = {'Ldia', 101};
static const ControlID kLdia_trace = {'Ldia', 102};
static const ControlID kLdia_detail = {'Ldia', 103};
static const ControlID kLdia_debug = {'Ldia', 104};
static const ControlID kLdia_open_close = {'Ldia', 105};

#define kLogDiaOpenClose_command 'Ldoc'



LogDialog::LogDialog() :
	AppDialogBase(CFSTR("GLEANApp"), CFSTR("LogDialog"))
{
	AppDialogBase::initialize_dialog(this);
}

void LogDialog::set_initial_dialog_data()
{
	ofstream& logfile = GleanApp::get_instance().get_logfile();
	
	if(logfile.is_open())
		set_button_label(kLdia_open_close, CFSTR("Close file"));
	else
		set_button_label(kLdia_open_close, CFSTR("Open file"));
	set_checkbox_value(kLdia_normal, Normal_out.is_present(logfile));
	set_checkbox_value(kLdia_trace, Trace_out.is_present(logfile));
//	set_checkbox_value(kLdia_detail, Detail_out.is_present(logfile));
//	set_checkbox_value(kLdia_debug, Debug_out.is_present(logfile));
}

void LogDialog::read_dialog_data()
{
	ofstream& logfile = GleanApp::get_instance().get_logfile();
	if(get_checkbox_value(kLdia_normal))
		Normal_out.add_stream(logfile);
	else
		Normal_out.remove_stream(logfile);

	if(get_checkbox_value(kLdia_trace))
		Trace_out.add_stream(logfile);
	else
		Trace_out.remove_stream(logfile);

/*	if(get_checkbox_value(kLdia_detail))
		Detail_out.add_stream(logfile);
	else
		Detail_out.remove_stream(logfile);
	if(get_checkbox_value(kLdia_debug))
		Debug_out.add_stream(logfile);
	else
		Debug_out.remove_stream(logfile);
*/

	GleanApp::get_instance().handle_trace_settings();
}

OSStatus LogDialog::handle_command(HICommand command)
{
	OSStatus result = eventNotHandledErr;

	switch(command.commandID) {
		case kLogDiaOpenClose_command: {
			ofstream& logfile = GleanApp::get_instance().get_logfile();
			if(logfile.is_open()) {
				logfile.close();
				}
			else {
				AppNavSaveAsDialog save_dialog;
				if(save_dialog.successful()) {
					string fullpathname = save_dialog.get_file_path();
					logfile.open(fullpathname.c_str());
					if(!logfile.is_open())
						SysBeep(1);
					}
				}
			if(logfile.is_open())
				set_button_label(kLdia_open_close, CFSTR("Close file"));
			else
				set_button_label(kLdia_open_close, CFSTR("Open file"));

			result = noErr;
			}
			break;
		}
	return result;
}

