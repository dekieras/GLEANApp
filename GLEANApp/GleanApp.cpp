/*
 *  GleanApp.cpp
 *
 *  Created by David Kieras on 11/15/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "GleanApp.h"
//#include "Windows.h"
#include "DisplayDialog.h"
#include "TraceDialog.h"
#include "RunDialog.h"
#include "BreakDialog.h"
#include "LogDialog.h"

#include "VisualView.h"
#include "WmView.h"
#include "ObjectView.h"
#include "TextView.h"
#include "AppNavDialogs.h"
//#include "MyAuditoryView.h"
//#include "CRunPeriodical.h"
//#include "MyTextView.h"
//#include "CNameTable.h"
//#include "String_Utilities.h"

#include "GLEANKernel/Model.h"
#include "GLEANKernel/Output_tee_globals.h"
//#include "PPS_globals.h"
//#include "Device_Processor.h"
#include "Dummy_Device.h"

#include "GLEANKernel/Device_base.h"

#include <unistd.h>
#include <sys/param.h>
#include <dlfcn.h> 

#include <climits>	// for LONG_MAX
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <exception>

using std::cout;	using std::endl;	using std::cerr;
using std::cin;
using std::ofstream;
using std::string;
using std::ostringstream;	using std::istringstream;
using std::copy;
using std::ostream_iterator;

// Control menu command constants
#define kLoadDeviceCommand	'Ldev'
#define kCompileCommand	'Comp'
#define kGoCommand		'Gocy'
#define kRunCommand		'Rcyc'
#define kPauseCommand	'Paus'
#define kHaltCommand	'Halt'
#define kRunDiaCommand 'Rdia'
#define kDispDiaCommand 'Ddia'
#define kTrDiaCommand	'Tdia'
#define kLogDiaCommand	'Ldia'
#define kBreakDiaCommand	'Bdia'

// the C++ types of the device class factories
typedef Device_base * create_device_fn_t();
typedef void destroy_device_fn_t(Device_base *);


// Meyer's Singleton
GleanApp& GleanApp::get_instance()
{
	static GleanApp app;
	return app;
}

//class Device_base;


GleanApp::GleanApp() : AppBase(CFSTR("GLEANApp")), model_ptr(0), device_ptr(0), device_lib_handle(0),
	run_state(UNREADY), run_duration_setting(RUN_FOR_TIME), run_continuously(false), refresh_setting(AFTER_EVERY_TIME),
	// integral time values are in ms
	sim_time_per_step(50), real_time_per_step(0.00001), 
	run_for_time(5000), run_until_time(600000), time_per_refresh(1000),
	run_result(false), run_time(0), run_time_limit(0), run_time_per_refresh(0),
	spatialview_scale(20), calibration_grid(false), report_workload(false), trace_text_view(0)
{
	initialize(this);
	
	try {
		model_ptr = new Model(device_ptr);
		}
	catch(Exception& x) {
		cerr << x.what() << endl;
		throw x;
		}
	catch (Assertion_exception& x) {
		cerr << x.what() << endl;
		throw x;
		}

	create_spatial_views();
	create_memory_views();
	create_text_views();
	
	{
//		chdir("/Users/kieras/Documents/Programming/EPICX/build/Release/");
		char buff[MAXPATHLEN+1];
		char * s = getcwd(buff, MAXPATHLEN);
		Normal_out << "cwd is " << s << endl;
	}

}

GleanApp::~GleanApp()
{
	if(device_ptr && device_lib_handle)
		destroy_and_unload_device();
}


// this function is called from AppWindowBase when a window gets closed
// dynamic cast here to convert from the base type to an unknown derived class
void GleanApp::window_closing(AppWindowBase * p)
{
	if(VisualView * sp = dynamic_cast<VisualView *>(p)) {
		model_ptr->remove_view(sp);
		spatial_views.erase(sp);
//		cout << "spatial view removed" << endl;
		return;
		}
	else if(MemoryView * op = dynamic_cast<MemoryView *>(p)) {
		model_ptr->remove_view(op);
		memory_views.erase(op);
		return;
		}
	else if(TextView * tp = dynamic_cast<TextView *>(p)) {
		model_ptr->remove_view(tp);
		text_views.erase(tp);
		if(tp == trace_text_view) {
			trace_text_view = 0;
			Trace_out.remove_view(tp);
			}
		Normal_out.remove_view(tp);
//		Detail_out.remove_view(tp);
//		Debug_out.remove_view(tp);
//		cout << "text view removed" << endl;
		}
	else if(dynamic_cast<AppDialogBase *>(p)) {
//		cout << "dialog removed" << endl;
		}
	else {
		assert(!"unknown view closed");;
		}
}

OSStatus GleanApp::HandleMenuUpdateStatus(HICommand command)
{
	OSStatus result = eventNotHandledErr;

	switch(command.commandID) {
		case kLoadDeviceCommand: 
			if(!model_ptr->get_device_ptr())
				EnableMenuCommand(NULL, kLoadDeviceCommand);
			else
				DisableMenuCommand(NULL, kLoadDeviceCommand);
		case kCompileCommand: 
			if(run_state == UNREADY || run_state == RUNNABLE)
				EnableMenuCommand(NULL, kCompileCommand);
			else
				DisableMenuCommand(NULL, kCompileCommand);
		case kGoCommand:
			if(run_state == RUNNABLE || run_state == PAUSED)
				EnableMenuCommand(NULL, kGoCommand);
			else
				DisableMenuCommand(NULL, kGoCommand);
			result = noErr;
			break;
		case kRunCommand:
			if(run_state == RUNNABLE || run_state == PAUSED)
				EnableMenuCommand(NULL, kRunCommand);
			else
				DisableMenuCommand(NULL, kRunCommand);
			result = noErr;
			break;
		case kPauseCommand:
			if(run_state == RUNNING)
				EnableMenuCommand(NULL, kPauseCommand);
			else
				DisableMenuCommand(NULL, kPauseCommand);
			result = noErr;
			break;
		case kHaltCommand:
			if(run_state == RUNNING || run_state == PAUSED)
				EnableMenuCommand(NULL, kHaltCommand);
			else
				DisableMenuCommand(NULL, kHaltCommand);
			result = noErr;
			break;
		case kRunDiaCommand:
			if(model_ptr->get_device_ptr())
				EnableMenuCommand(NULL, kRunDiaCommand);
			else
				DisableMenuCommand(NULL, kRunDiaCommand);
			result = noErr;
			break;
		case kDispDiaCommand:
			EnableMenuCommand(NULL, kDispDiaCommand);
			result = noErr;
			break;
		case kTrDiaCommand:
			if(model_ptr->get_device_ptr())
				EnableMenuCommand(NULL, kTrDiaCommand);
			else
				DisableMenuCommand(NULL, kTrDiaCommand);
			result = noErr;
			break;
		case kLogDiaCommand:
			EnableMenuCommand(NULL, kLogDiaCommand);
			result = noErr;
			break;
		case kBreakDiaCommand:
			if(model_ptr->get_compiled())
				EnableMenuCommand(NULL, kBreakDiaCommand);
			else
				DisableMenuCommand(NULL, kBreakDiaCommand);
			result = noErr;
			break;
		}
	return result;
}


OSStatus GleanApp::HandleMenuProcessCommand(HICommand command)
{
	OSStatus result = eventNotHandledErr;

	// we'll catch our exceptions here
	try {
	switch(command.commandID) {
		case kLoadDeviceCommand: 
			do_load_device_dialog();
			result = noErr;
			break;
		case kCompileCommand: 
			do_compile_dialog();
			result = noErr;
			break;
		case kGoCommand:
			do_go_command();
			result = noErr;
			break;
		case kRunCommand:
			do_run_command();
			result = noErr;
			break;
		case kPauseCommand:
			do_pause_command();
			result = noErr;
			break;
		case kHaltCommand:
			do_halt_command();
			result = noErr;
			break;
		case kRunDiaCommand:
			do_run_settings_dialog();
			result = noErr;
			break;
		case kDispDiaCommand:
			do_display_dialog();
			result = noErr;
			break;
		case kTrDiaCommand:
			do_trace_dialog();
			result = noErr;
			break;
		case kLogDiaCommand:
			do_log_dialog();
			result = noErr;
			break;
		case kBreakDiaCommand:
			do_break_dialog();
			result = noErr;
			break;
		}	// end of command switch
		}	// end of try block
	// catch the exceptions here
	catch(Exception& x) {
		Normal_out << "Error: " << x.what() << endl;
		}
	// This is my own Assert exception
	catch (Assertion_exception& x) {
		Normal_out << x.what() << endl;
		}
	// show any standard exceptions and then re-throw
	catch (std::exception& x) {
		Normal_out << x.what() << endl;
//		throw;
		}
	return result;
}

void GleanApp::do_load_device_dialog()
{
	AppNavOpenDialog open_dialog(false);	// false to switch selection to dylibs only
	if(open_dialog.successful()) {
		Normal_out << "file name is \"" << open_dialog.get_file_name() << "\"" << endl;
		current_directory = open_dialog.get_directory_path();
		Normal_out << "directory path is \"" << current_directory << "\"" << endl;
		string fullpathname = open_dialog.get_file_path();
		Normal_out << "file path is \"" << fullpathname << "\"" << endl;
		{
		chdir(current_directory.c_str());
		char buff[MAXPATHLEN+1];
		char * s = getcwd(buff, MAXPATHLEN);
		Normal_out << "cwd is now " << s << endl;
		}
		
		load_and_create_device(fullpathname);
		model_ptr->set_device_ptr(device_ptr);
		}
	else {
		Normal_out << "No file selected" << endl;
		}
	run_state = (model_ptr->get_device_ptr() && model_ptr->get_compiled()) ? RUNNABLE : UNREADY;
}

void GleanApp::do_compile_dialog()
{
	AppNavOpenDialog open_dialog;
	if(open_dialog.successful()) {
		Normal_out << "file name is \"" << open_dialog.get_file_name() << "\"" << endl;
		current_directory = open_dialog.get_directory_path();
		Normal_out << "directory path is \"" << current_directory << "\"" << endl;
		string fullpathname = open_dialog.get_file_path();
		Normal_out << "file path is \"" << fullpathname << "\"" << endl;
		{
		chdir(current_directory.c_str());
		char buff[MAXPATHLEN+1];
		char * s = getcwd(buff, MAXPATHLEN);
		Normal_out << "cwd is now " << s << endl;
		}
		model_ptr->set_gomsl_filename(fullpathname);
		model_ptr->compile();
		}
	else {
		Normal_out << "No file selected" << endl;
		}
	run_state = (model_ptr->get_device_ptr() && model_ptr->get_compiled()) ? RUNNABLE : UNREADY;
}

void GleanApp::do_go_command()
{
	// run one step, then refresh the views
	run_result = model_ptr->run_time(sim_time_per_step);
	refresh_spatial_views();
	refresh_memory_views();
	if(!run_result) 
		run_state = RUNNABLE;	// run was completed, model is stopped
	else
		run_state = PAUSED;
}

void GleanApp::do_run_command()
{	
	long current_time = 0;
	// in either state RUNNABLE or PAUSED at this point
	switch(run_state) {
		case RUNNABLE:	// starting up - model automatically initializes
			switch(run_duration_setting) {
				case RUN_FOR_TIME:
					run_time_limit = run_for_time;
					break;
				case RUN_UNTIL_TIME:
				case RUN_UNTIL_DONE:
				default:
					run_time_limit = 0;
					break;
				}
			break;
		case PAUSED:	// we were already running - we might have changed settings
			current_time = model_ptr->get_time();
			switch(run_duration_setting) {
				case RUN_FOR_TIME:
					// go again if we ran for the specified amount of time or we paused and then changed
					if(current_time >= run_time_limit)
						run_time_limit = current_time + run_for_time;	// go again
					break;
				case RUN_UNTIL_TIME:
					// are we past the specified time or did we pause and change from run until done?
					if(current_time >= run_until_time) {
						SysBeep(1);		// don't let the user go again without changing settings
						return;
						}
					run_time_limit = 0;
					break;
				case RUN_UNTIL_DONE:
				default:
					run_time_limit = 0;
					break;
				}
		default:
			break;
		}
		
	switch(refresh_setting) {
		case EACH_STEP:
			run_time_per_refresh = sim_time_per_step;
			break;
		case AFTER_EVERY_TIME:
			run_time_per_refresh = time_per_refresh;
			break;
		case NONE_DURING_RUN:
			run_time_per_refresh = LONG_MAX;
			break;
		default:
			break;
		}
	
	run_state = RUNNING;
	if(run_continuously) {
		do_run_continuously();	// no timer
		}
	else {
		timer.start(real_time_per_step, real_time_per_step);
		}
}

// run all the steps, then refresh the views
void GleanApp::do_run_continuously()
{
	long current_time = 0;
	do {
		run_result = model_ptr->run_time(sim_time_per_step);
		current_time = model_ptr->get_time();
		} while (run_result && (
			(run_duration_setting == RUN_FOR_TIME && current_time < run_time_limit) ||
			(run_duration_setting == RUN_UNTIL_TIME && current_time < run_until_time) ||
			(run_duration_setting == RUN_UNTIL_DONE)
			)
		);
	refresh_spatial_views();
	refresh_memory_views();
	if(!run_result) 
		run_state = RUNNABLE;	// run was completed, model is stopped
	else
		run_state = PAUSED;
	if(run_state == RUNNABLE) {
		if(report_workload)
			model_ptr->output_workload_reports();
		Normal_out << "Run Completed!" << endl;
		}
}

// this is called via a system call-back, so catch exceptions here
void GleanAppTimer::handle_timer_event()
{
//	SysBeep(.1);
	try {
		GleanApp::get_instance().run_next_cycle_and_refresh_periodically();
		}	// end of try block
	// catch the exceptions here
	catch(Exception& x) {
		Normal_out << "Error: " << x.what() << endl;
		cerr << "Error: " << x.what() << endl;
		GleanApp::get_instance().halt_run();
		}
	// This is my own Assert exception
	catch (Assertion_exception& x) {
		Normal_out << x.what() << endl;
		cerr << "Error: " << x.what() << endl;
		GleanApp::get_instance().halt_run();
		}
	// show any standard exceptions
	catch (std::exception& x) {
		Normal_out << x.what() << endl;
		cerr << "Error: " << x.what() << endl;
		GleanApp::get_instance().halt_run();
		}
}

void GleanApp::run_next_cycle_and_refresh_periodically()
{
	assert(run_state == RUNNING);
	run_result = model_ptr->run_time(sim_time_per_step);
	long current_time = model_ptr->get_time();
	if(run_result && !(model_ptr->is_next_step_break_set()) && (
			(run_duration_setting == RUN_FOR_TIME && current_time < run_time_limit) ||
			(run_duration_setting == RUN_UNTIL_TIME && current_time < run_until_time) ||
			(run_duration_setting == RUN_UNTIL_DONE)
			)
		) {
			if(current_time % run_time_per_refresh == 0) {
				refresh_spatial_views();
				refresh_memory_views();
				}
			}
	else {
		timer.stop();
		refresh_spatial_views();
		refresh_memory_views();
		// is the run complete? if true returned, can keep running so we are paused
		run_state = (run_result) ? PAUSED : RUNNABLE; 
		if(run_state == PAUSED) {
			if(model_ptr->is_next_step_break_set())
				Normal_out << "Break at next step" << endl;
			else if(run_duration_setting == RUN_UNTIL_TIME || run_duration_setting == RUN_FOR_TIME)
				Normal_out << "Run command complete" << endl;
			}
		if(run_state == RUNNABLE) {
			if(report_workload)
				model_ptr->output_workload_reports();
			Normal_out << "Run Completed!" << endl;
			}
		}
}

void GleanApp::do_pause_command()
{
	assert(run_state == RUNNING);
	timer.stop();
	run_state = PAUSED;
	Normal_out << "Run paused" << endl;
}

void GleanApp::halt_run()
{
	do_halt_command();
}

void GleanApp::do_halt_command()
{
	assert(run_state == RUNNING || run_state == PAUSED);
	if(run_state == RUNNING) {
		timer.stop();
		}
	model_ptr->stop();
	run_state = RUNNABLE;
	Normal_out << "Run halted" << endl;
}


void GleanApp::do_display_dialog()
{
	DisplayDialog * dialog_ptr = new DisplayDialog;
	add_window(dialog_ptr);
}

void GleanApp::do_run_settings_dialog()
{
	RunDialog * dialog_ptr = new RunDialog;
	add_window(dialog_ptr);
}

void GleanApp::do_trace_dialog()
{
	TraceDialog * dialog_ptr = new TraceDialog;
	add_window(dialog_ptr);
}

// called from TraceDialog to act on Dialog settings
void GleanApp::handle_trace_settings()
{
	bool trace_on = 
		model_ptr->get_trace_visual() || 
		model_ptr->get_trace_auditory() ||
		model_ptr->get_trace_cognitive() || 
		model_ptr->get_trace_WM() || 
		model_ptr->get_trace_task() || 
		model_ptr->get_trace_manual() || 
		model_ptr->get_trace_vocal() ||
		model_ptr->get_trace_highlevel() ||
		model_ptr->get_trace_device();
	
	if(trace_on && !trace_text_view) {
		View_base * evp;
		trace_text_view = create_text_window(CFSTR("Trace output"), evp, 300, 500);
		Trace_out.add_view(trace_text_view);
		Trace_out << "Trace!" << endl;
		}
	if(!trace_on && trace_text_view) {
		close_window(trace_text_view);
		trace_text_view = 0;
		}

}

void GleanApp::do_log_dialog()
{
	LogDialog * dialog_ptr = new LogDialog;
	add_window(dialog_ptr);
}

void GleanApp::do_break_dialog()
{
	BreakDialog * dialog_ptr = new BreakDialog;
	add_window(dialog_ptr);
}

void GleanApp::create_text_views()
{
	TextView * tvp;
	View_base * evp;
	tvp = create_text_window(CFSTR("Normal output"), evp, 0, 500);
	Normal_out.add_view(evp);
	Normal_out << "Normal Output!" << endl;
}


TextView * GleanApp::create_text_window(CFStringRef title, View_base *& evp, int hpos, int vpos)
{
	TextView * tvp = new TextView(title);
	add_window(tvp);						// tell the base application object about this window
	text_views.insert(tvp);					// put in our list of text views
	evp = dynamic_cast<View_base *>(tvp);	// convert to other base
	assert(evp);	// should always work
	model_ptr->add_view(evp);
	
	tvp->move_to(hpos, vpos);
	tvp->show();
	return tvp;
}
			

void GleanApp::create_spatial_views()
{
	VisualView * svp;
	View_base * evp;
	svp = create_spatial_window(CFSTR("Visual Processor"), evp, 0, 100);
	model_ptr->add_visual_physical_view(evp);
}


VisualView * GleanApp::create_spatial_window(CFStringRef title, View_base *& evp, int hpos, int vpos)
{
	VisualView * svp = new VisualView(title);
	add_window(svp);						// tell the base application object about this window
	spatial_views.insert(svp);				// put in our list of spatial views
	evp = dynamic_cast<View_base *>(svp);	// convert to other base
	assert(evp);	// should always work
	
	svp->set_scale(spatialview_scale);
	svp->set_grid_on(calibration_grid);
	svp->move_to(hpos, vpos);
	svp->show();

	return svp;
}
			
// refresh the views after telling them the time and marking them as changed
void GleanApp::refresh_spatial_views()
{
	for(spatialviews_t::iterator it = spatial_views.begin(); it != spatial_views.end(); ++it) {
		VisualView * svp = static_cast<VisualView *>(*it);
		svp->notify_time(model_ptr->get_time());
		svp->set_changed();
		svp->refresh();
		}
}

// these are callable externally
void GleanApp::set_spatialview_scale(int scale)
{
	spatialview_scale = scale;
	for(spatialviews_t::iterator it = spatial_views.begin(); it != spatial_views.end(); ++it) {
		VisualView * svp = static_cast<VisualView *>(*it);
		svp->set_scale(spatialview_scale);
		}
	refresh_spatial_views();
}

void GleanApp::set_calibration_grid(bool state)
{
	calibration_grid = state;
	for(spatialviews_t::iterator it = spatial_views.begin(); it != spatial_views.end(); ++it) {
		VisualView * svp = static_cast<VisualView *>(*it);
		svp->set_grid_on(state);
		}
	refresh_spatial_views();
}

void GleanApp::create_memory_views()
{
	View_base * evp;
	create_wm_window(CFSTR("WM State"), evp, 100, 100);
	model_ptr->add_wm_view(evp);

	create_object_window(CFSTR("Task Memory State"), evp, 200, 100);
	model_ptr->add_task_view(evp);
}

MemoryView * GleanApp::create_object_window(CFStringRef title, View_base *& evp, int hpos, int vpos)
{
	MemoryView * ovp = new ObjectView(title);
	add_window(ovp);						// tell the base application object about this window
	memory_views.insert(ovp);				// put in our list of object views
	evp = dynamic_cast<View_base *>(ovp);	// convert to other base
	assert(evp);	// should always work
	
	ovp->move_to(hpos, vpos);
	ovp->show();

	return ovp;
}
MemoryView * GleanApp::create_wm_window(CFStringRef title, View_base *& evp, int hpos, int vpos)
{
	MemoryView * ovp = new WmView(title);
	add_window(ovp);						// tell the base application object about this window
	memory_views.insert(ovp);				// put in our list of object views
	evp = dynamic_cast<View_base *>(ovp);	// convert to other base
	assert(evp);	// should always work
	
	ovp->move_to(hpos, vpos);
	ovp->show();

	return ovp;
}
		
// refresh the views after telling them the time and marking them as changed
void GleanApp::refresh_memory_views()
{
	for(memory_views_t::iterator it = memory_views.begin(); it != memory_views.end(); ++it) {
		MemoryView * ovp = static_cast<MemoryView *>(*it);
		ovp->notify_time(model_ptr->get_time());
		ovp->set_changed();
		ovp->refresh();
		}
}



void GleanApp::load_and_create_device(const std::string& fullpathname)
{
	// load the device library
	device_lib_handle = dlopen(fullpathname.c_str(), RTLD_LAZY);
	if (!device_lib_handle) {
		cerr << "Failed to load library: " << fullpathname << ' ' << dlerror() << endl;
//		return 1;
		}
 
	// reset errors
	dlerror();
	// load the symbols
	// Even the C-style cast (to go with the C-style dlsym interface) does not suppress a pointer conversion warning (in gcc 4.0)
	// The problem is that dlsym returns a void *, which by Standard, can't be converted to a function pointer because
	// platforms may differ in the size of a data pointer vs a function pointer.
//	create_device_fn_t * create_device_fn = (create_device_fn_t*) dlsym(device_lib_handle, "create_device");
	create_device_fn_t * create_device_fn = reinterpret_cast<create_device_fn_t*> (dlsym(device_lib_handle, "create_device"));
	const char * dlsym_error = dlerror();
	if (dlsym_error) {
		cerr << "Cannot load symbol create_device: " << dlsym_error << endl;
//		return 1;
		}
    
	// create an instance of the class
	device_ptr = create_device_fn();
}


void GleanApp::destroy_and_unload_device()
{
	// Even the C-style cast (to go with the C-style dlsym interface) does not suppress a pointer conversion warning (in gcc 4.0)
	// The problem is that dlsym returns a void *, which by Standard, can't be converted to a function pointer because
	// platforms may differ in the size of a data pointer vs a function pointer.
//	destroy_device_fn_t * destroy_device_fn = (destroy_device_fn_t*) dlsym(device_lib_handle, "destroy_device");
	destroy_device_fn_t * destroy_device_fn = reinterpret_cast<destroy_device_fn_t*> (dlsym(device_lib_handle, "destroy_device"));
	const char * dlsym_error = dlerror();
	if (dlsym_error) {
		cerr << "Cannot load symbol destroy_device: " << dlsym_error << endl;
//		return 1;
		}

	// destroy the device instance	
	destroy_device_fn(device_ptr);
	device_ptr = 0;
	
	// unload the library
	dlclose(device_lib_handle);
	device_lib_handle = 0;
}



