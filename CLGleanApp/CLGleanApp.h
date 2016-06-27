#ifndef CLGLEANAPP_H
#define CLGLEANAPP_H

#include <string>

class Model;

class Device_base;

class CLGleanApp {
public:
	CLGleanApp(Device_base * device);
	virtual ~CLGleanApp();
	
	virtual Model* get_model() { return model_ptr; }
	virtual void run_top_level();
	virtual void quit() {}

private:
	Model * model_ptr;
	Device_base * device_ptr;	// pointer to the current device - set by load_device()s
	void * device_lib_handle;	// pointer to loaded library
	
	// UI state and settings
	bool continue_running;
	bool do_commands;
	bool single_step;
	bool trace_output;
	bool stop_after_steps;
	int steps_to_stop;
	bool stop_at_time;
	long time_to_stop;
	bool workload_reports;
	bool output_label_only;
	bool output_list_step;
	bool output_wm_contents;
	
	void run_simulation();
	bool run_cycle();
	void set_break(bool state);
	void turn_off_output();
	void save_output_settings();
	void set_output_settings();

	// device plugin functions
	void load_device();
	void load_and_create_device(const std::string& fullpathname);
	void destroy_and_unload_device();

};


#endif
