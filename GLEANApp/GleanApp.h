/*
 *  GleanApp.h
 *
 *  Created by David Kieras on 11/15/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef EPICAPP_H
#define EPICAPP_H
#include <Carbon/Carbon.h>
#include "AppBase.h"
#include "AppTimer.h"

#include <set>
#include <string>
#include <fstream>
#include <algorithm>
#include <functional>

class AppWindow;
class TextView;
class VisualView;
class MemoryView;
class AuditoryView;
class Model;
class View_base;
class Device_base;

using std::mem_fun;
using std::for_each;

// A timer for running the run loop
class GleanAppTimer : public AppTimer {
public:	
	GleanAppTimer() {AppTimer::initialize(this);}
	virtual void handle_timer_event();
};


// A singleton class
class GleanApp : public AppBase {
public:
	static GleanApp& get_instance();

	Model * get_model_ptr() const	// valid only after this object is created
		{return model_ptr;}
	Model & get_model() const
		{return *model_ptr;}
		
	/* accessors for use by dialogs */
	
	// enums for application control state
	enum Run_duration_e {SINGLE_STEP, RUN_FOR_TIME, RUN_UNTIL_TIME, RUN_UNTIL_DONE, RUN_CONTINUOUSLY};
	enum Refresh_e {EACH_STEP, AFTER_EVERY_TIME, NONE_DURING_RUN};
	
	enum Run_duration_e get_run_duration_setting() const
		{return run_duration_setting;}
	void set_run_duration_setting(Run_duration_e run_duration_setting_)
		{run_duration_setting = run_duration_setting_;}
	enum Refresh_e get_refresh_setting() const
		{return refresh_setting;}
	void set_refresh_setting(Refresh_e refresh_setting_)
		{refresh_setting = refresh_setting_;}
		
	bool get_run_continuously() const
		{return run_continuously;}
	void set_run_continuously(bool run_continuously_)
		{run_continuously = run_continuously_;}

	long get_sim_time_per_step() const 
		{return sim_time_per_step;}
	void set_sim_time_per_step(long sim_time_per_step_)
		{sim_time_per_step = sim_time_per_step_;}

	double get_real_time_per_step() const 
		{return real_time_per_step;}
	void set_real_time_per_step(double real_time_per_step_)
		{real_time_per_step = real_time_per_step_;}
		
	long get_run_for_time() const 
		{return run_for_time;}
	void set_run_for_time(long run_for_time_)
		{run_for_time = run_for_time_;}

	long get_run_until_time() const 
		{return run_until_time;}
	void set_run_until_time(long run_until_time_)
		{run_until_time = run_until_time_;}

	long get_time_per_refresh() const 
		{return time_per_refresh;}
	void set_time_per_refresh(long time_per_refresh_)
		{time_per_refresh = time_per_refresh_;}

	int get_spatialview_scale() const {return spatialview_scale;}
	void set_spatialview_scale(int scale);
	
	bool get_calibration_grid() const {return calibration_grid;}
	void set_calibration_grid(bool state);

	bool get_report_workload() const {return report_workload;}
	void set_report_workload(bool state)
		{report_workload = state;}

	void handle_trace_settings();
	std::ofstream& get_logfile() {return logfile;}

	// accessed by the run timer
	void run_next_cycle_and_refresh_periodically();
	void halt_run();
/*
	void add_spatialview(VisualView *);
	void remove_spatialview(VisualView *);
	void add_auditoryview(AuditoryView *);
	void remove_auditoryview(AuditoryView *);
	void add_textview(TextView *);
	void remove_textview(TextView *);
//	void pane_destructing(LPane * pane_ptr);
*/

protected:
	// this function is called from the AppBase when the window is closing
	virtual void window_closing(AppWindowBase * p);

	virtual OSStatus HandleMenuUpdateStatus(HICommand command);
	virtual OSStatus HandleMenuProcessCommand(HICommand command);

private:
	GleanApp();
	~GleanApp();

	Model * model_ptr;	// pointer to the model object - initialized during construction	
	Device_base * device_ptr;	// pointer to the current device - set by load_device()s
	void * device_lib_handle;	// pointer to loaded library
	
	enum Run_state_e {UNREADY, RUNNABLE, RUNNING, PAUSED};
	// control variables
	Run_state_e run_state;
	Run_duration_e run_duration_setting;
	Refresh_e refresh_setting;
	bool run_continuously;
	long sim_time_per_step;	// e.g. 50 for 50 ms
	double real_time_per_step;	// e.g. .05 for "real time" rate
	long run_for_time;
	long run_until_time;
	long time_per_refresh;		// 200 for every fourth cycle
	
	bool run_result;
	long run_time;				// keeps time (in ms) from Run command
	long run_time_limit;
	long run_time_per_refresh;

	int spatialview_scale;
	bool calibration_grid;
	double spatialview_mag_factor;
	bool report_workload;		// whether to generate workload report

	/*** Helper functions ***/

	// managing model views
	void create_spatial_views();
	void create_memory_views();
//	void create_auditory_views();
	void create_text_views();

	// create the windows, and return the three pointers needed to refer to it in all respects
	VisualView * create_spatial_window(CFStringRef title, View_base *& theEpicView, int hpos, int vpos);
	MemoryView * create_object_window(CFStringRef title, View_base *& theEpicView, int hpos, int vpos);
	MemoryView * create_wm_window(CFStringRef title, View_base *& theEpicView, int hpos, int vpos);
//	AppWindow * create_auditory_window(AuditoryView *& theAuditoryView, View_base *&theEpicView, int hpos, int vpos);
	TextView * create_text_window(CFStringRef title, View_base *&theEpicView, int hpos, int vpos);
	void set_spatialview_mag(double mag);
	void refresh_spatial_views();
	void refresh_memory_views();

	// we have to keep track of spatial views in order to control their scaling,
	// also we may have to remove them from the model separately?
	typedef std::set<AppWindowBase *> spatialviews_t; // set for uniqueness
	spatialviews_t spatial_views;
	typedef std::set<AppWindowBase *> memory_views_t; // set for uniqueness
	memory_views_t memory_views;
	// ditto for auditory views
	typedef std::set<AuditoryView *> auditoryviews_t; // set for uniqueness
	auditoryviews_t auditory_views;
	// we have to keep track of the textviews in order to tell the model about them
	typedef std::set<TextView *> textviews_t; // set for uniqueness
	textviews_t text_views;
	
	// a special holder for the trace view
	TextView * trace_text_view;
	
	bool logging_enabled;
	std::string outfile_name;
	// an output file stream for logging
	std::ofstream logfile;
	std::string current_directory;	// holds the current directory path; set in Compile command

	// command functions
	void do_go_command();
	void do_run_command();
	void do_run_continuously();
	void do_pause_command();
	void do_halt_command();
	void do_quit_command();

	// functions for dialog boxes
	void do_load_device_dialog();
	void do_compile_dialog();
	void do_display_dialog();
	void do_run_settings_dialog();
	void do_trace_dialog();
	void do_log_dialog();
	void do_break_dialog();
	
	// device plugin functions
	void load_and_create_device(const std::string& fullpathname);
	void destroy_and_unload_device();

//	bool get_specs_for_input(const char * title, FSSpec& fs, std::string& fullpathname);
//	bool get_specs_for_output(std::string& short_filename, FSSpec& fs, std::string& fullpathname);
//	void open_ifstream(const char * title, std::ifstream& infile, std::string& fullpathname);

	GleanAppTimer timer;
};

#endif

