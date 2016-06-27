#include "CLGleanApp.h"

#include "GLEANKernel/Model.h"
#include "GLEANKernel/Glean_exceptions.h"
#include "GLEANKernel/Assert_throw.h"
#include "GLEANKernel/Output_tee_globals.h"
#include "GLEANKernel/Symbol.h"

#include <dlfcn.h> 

#include <iostream>
#include <string>
#include <cmath>
using std::cout;	using std::endl;	using std::cerr;	using std::cin;
using std::string;
using std::srand;

namespace
{
   const long Single_cycle_duration_c = 50;
}

CLGleanApp::CLGleanApp(Device_base * device) :
	model_ptr(0), device_ptr(0), device_lib_handle(0),
	continue_running(true), do_commands(true), single_step(true),trace_output(false),
	stop_after_steps(false), steps_to_stop(0), stop_at_time(false), time_to_stop(0),
	workload_reports(false), output_label_only(true), output_list_step(false), output_wm_contents(false)
{
	Normal_out.add_stream(cout);	// Normal_out now copies to cout
		
	try {
		model_ptr = new Model(device);
		
		// initialize to standard defaults - need better function names here!
		model_ptr->set_output_compiler_details(false);		// output listing of model
		model_ptr->set_output_compiler_messages(false);		// output learning data
		set_output_settings();	// copy our settings variables into model's
		
		}
	catch(Exception& x) {
		cerr << x.what() << endl;
		throw x;
		}
	catch (Assertion_exception& x) {
		cerr << x.what() << endl;
		throw x;
		}
	catch (...) {
		cerr << "Unknown exception caught" << endl;
		throw;
		}
}

CLGleanApp::~CLGleanApp()
{
	delete model_ptr;
	destroy_and_unload_device();	
}

void CLGleanApp::run_top_level()	// top-level command-handling loop
{
	load_device();
	model_ptr->set_device_ptr(device_ptr);

	char command;
	bool run = true;
	while (run) {
	try {
		Normal_out << "Enter command: f, c, l, d, r, s, t, w, q, R, ?: ";
		cin >> command;
		switch (command) {
			case 'f': {
				// ask for filename
				Normal_out << "Enter a gomsl filename (no embedded spaces): ";
				string fn;
				cin >> fn;
				model_ptr->set_gomsl_filename(fn);
				}
				break;
			case 'c':
				if(model_ptr->get_gomsl_filename_set())
					model_ptr->compile();
				break;
			case 'l':
				if (model_ptr->get_output_compiler_messages()) {
					model_ptr->set_output_compiler_messages(false);
					Normal_out << "Learning calculation will not be output" << endl;
					}
				else {
					model_ptr->set_output_compiler_messages(true);
					Normal_out << "Learning calculation will be output" << endl;
					}
				break;
			case 'd':
				if (model_ptr->get_output_compiler_details()) {
					model_ptr->set_output_compiler_details(false);
					Normal_out << "GOMS model listing will not be output" << endl;
					}
				else {
					model_ptr->set_output_compiler_details(true);
					Normal_out << "GOMS model listing will be output" << endl;
					}
				break;
			case 'r':	// always start in single-step mode
				single_step = true;
				model_ptr->set_output_run_messages(true);	// which steps are executing
				Normal_out << "single_step mode is enabled" << endl;
				run_simulation();
				break;
			case 's':
				if (single_step) {
					single_step = false;
					model_ptr->set_output_run_messages(false);	// which steps are executing
					Normal_out << "single_step mode is disabled" << endl;
					}
				else {
					single_step = true;
					model_ptr->set_output_run_messages(true);	// which steps are executing
					Normal_out << "single_step mode is enabled" << endl;
					}
				break;
			case 't':
				if (trace_output) {
					trace_output = false;
					Trace_out.remove_stream(cout);		// Trace_out now empty
					Normal_out << "trace output is disabled" << endl;
					}
				else {
					trace_output = true;
					Trace_out.add_stream(cout);		// Trace_out now copies to cout
					Normal_out << "trace output is enabled" << endl;
					}
				break;
			case 'w':
				if (workload_reports) {
					workload_reports = false;
					Normal_out << "Workload reports will not be output" << endl;
					}
				else {
					workload_reports = true;
					Normal_out << "Workload reports will be output" << endl;
					}
				break;
			case 'q':
				Normal_out << "Quitting" << endl;
				run = false;
				break;
			case 'R':
				// ask for random seed
				Normal_out << "Enter a random generator seed value (integer): ";
				unsigned seed;
				cin >> seed;
				Normal_out << "Random seed: " << seed << endl;
				srand(seed);
				break;
			default:
				Normal_out << "Invalid command!" << endl;
			case '?':	// output list of commands
				Normal_out << "Valid commands:\n"
					<< "f(ile) - specify method file\n"
					<< "c(ompile) - compile method file\n"
					<< "l(earning analysis) - produce learning time analysis\n"
					<< "d(isplay) - show contents of GOMS model\n"
					<< "r(un) - execute GOMS model\n"
					<< "s(ingle step toggle) - change single step mode\n"
					<< "t(race output toggle) - change trace output\n"
					<< "w(orkload reports toggle) - change workload reports mode\n"
					<< "q(uit) - exit\n"
					<< "R(andom seed) - set the random generator seed\n"
					<< "? - this list"
					<< endl;
				break;
			}
		} // end of try block
		catch (Glean_exception& x)
			{
				Normal_out << x.what() << endl;
				Normal_out << "Model execution halted" << endl;
			}
		catch (...) {
			cerr << "Unknown exception caught" << endl;
			throw;
			}
		} // end of while loop
}



// start the simulation running, doing the single-step after each.
void CLGleanApp::run_simulation()
{		
	if(single_step)
		Normal_out << "Will pause after each significant step" << endl;

	bool label_only = model_ptr->get_output_run_messages();
	bool list_step = model_ptr->get_output_run_details();
	if(label_only && !list_step) {
		Normal_out << "Step labels only will be listed" << endl;
		}
	else if(!label_only && list_step) {
		Normal_out << "Step contents will be listed" << endl;
		}
	else {
		Normal_out << "Step output will not be shown" << endl;
		}
	
	model_ptr->initialize();

	do_commands = true;
	while(run_cycle()) {}
	// output final workload reports
	if(workload_reports)
		model_ptr->output_workload_reports();
}


bool CLGleanApp::run_cycle()
{	
	while(do_commands) {
	try {
	
	cout << model_ptr->get_time() << ":->";
	char command;
	cin >> command;
	switch(command) {
		case 'n':	// go to next halt
			do_commands = false;
			break;
		case 'N': {	// continue until a number of steps have gone by
			int n;
			while(!(cin >> n) && n < 0) {
				cin.clear();
				while(cin.get() != '\n') {}// skip rest of line
				cout << "Enter number of steps to do, please: ";
				}
			steps_to_stop = n;
			stop_after_steps = true;
			single_step = false;
			do_commands = false;
			}
			break;
		case 't':
		case 'T': {	// set a time to continue until
			long n;
			while(!(cin >> n) && n < 0) {
				cin.clear();
				while(cin.get() != '\n') {}// skip rest of line
				cout << "Enter time (ms) to run until, please: ";
				}
			time_to_stop = n;
			stop_at_time = true;
			single_step = false;
			if (command == 'T') {
				save_output_settings();
				turn_off_output();
				}
			do_commands = false;
			}
			break;
		case 'G':	// do not stop or output any more
			single_step = false;
			save_output_settings();
			turn_off_output();
			do_commands = false;
			break;
		case 'g':	// do not stop any more, but continue to show output if set
			single_step = false;
			do_commands = false;
			break;
		case 'b':
			// set a break point on named step and enable the breaks if not already
			set_break(true);
			if(!model_ptr->get_breaks_enabled()) {
				model_ptr->set_breaks_enabled(true);
				Normal_out << "Breaks enabled" << endl;
				}				
			break;
		case 'u': 	// unset a break point on named step
			set_break(false);
			break;
		case 'B':	// enable all breaks
			model_ptr->set_breaks_enabled(true);
			Normal_out << "Breaks enabled" << endl;
			break;
		case 'U':	// disable all breaks (still in place)
			model_ptr->set_breaks_enabled(false);
			Normal_out << "Breaks disabled" << endl;
			break;
		case 'q':	// terminate execution
		case 'H':	// terminate execution
			Normal_out << " Simulation terminating" << endl;
			// stop the stimulation
			model_ptr->stop();
			return false;	// exit this function
			break;
		// set display options - continue until changed ...
		case 'd': {// display ... 
			cin >> command;
			switch(command) {
				case 'l': 		// labels only
					output_label_only = true;
					output_list_step = false;
					set_output_settings();
					Normal_out << "Step labels will be listed" << endl;
					break;
				case 'c': 		// step contents
					output_label_only = false;
					output_list_step = true;
					set_output_settings();
					Normal_out << "Step contents will be listed" << endl;
					break;
				case 'w': 		// WM info after each step
					output_wm_contents = true;
					set_output_settings();
					Normal_out << "Step WM output will be shown" << endl;
					break;
				case 'n': 		// no step information
					output_label_only = false;
					output_list_step = false;
					output_wm_contents = false;
					set_output_settings();
					Normal_out << "Step and WM output will not be shown" << endl;
					break;
				default:
					Normal_out << "Invalid command!" << endl;
					break;
				}
			break;
			}
//		case 'p':	// show busy/free status of processors
//			show_all_processor_status();
//			break;
		case 's':	// show info designated by second letter
			cin >> command;
			switch(command) {
				/* testing only
				case 'f':	{	// show the file information about the current (next) step
					Normal_out << "File: " << model_ptr->get_main_current_step_file_name() 
						<< " Line: " << model_ptr->get_main_current_step_line_number() << endl;
					break;
					}
				*/
				case 'n':
					model_ptr->output_steps();	// outputs all thread steps
					break;
				case 'w':
					model_ptr->output_tag_store_contents();
					break;
				case 'o':
					model_ptr->output_object_store_contents();
					break;
				case 'v':
					model_ptr->output_visual_memory_contents();
					break;
				case 'a':
					model_ptr->output_auditory_memory_contents();
					break;
				case 't':
					model_ptr->output_task_memory_contents();
					break;
				case 'l':
					model_ptr->output_ltm_memory_contents();
					break;
				default:
					Normal_out << "Invalid command!" << endl;
					break;
				}
			break;
		default:
			Normal_out << "Invalid command!" << endl;
		case '?':	// output list of commands
			Normal_out << "Valid commands:\n"
				<< "n(ext) - advance to next step\n"
				<< "N(ext) n - stop after n steps\n"
				<< "T(ime) t - advance until time t elapsed\n"
				<< "g(o) - continue without pausing, showing step output\n"
				<< "G(o) - continue without pausing, without step output\n"
				<< "b(reak) - set a break point as given by next letter and enable breaks:\n"
				<< "   n(next main thread step), s(pecified method step), f(ile name and line)\n"
				<< "u(nbreak) - reset a break point as given by next letter:\n"
				<< "   n(next main thread step), s(pecified method step), f(ile name and line)\n"
				<< "B(reak) - enable all existing breakpoints"
				<< "U(nbreak) - disable all existing breakpoints without removing them\n"
				<< "H(alt) or q(uit) - terminate simulation\n"
				<< "d(isplay) - set step display mode given by next letter:\n"
				<< "   l(abel for step), c(ontents of step), w(m tag store), n(o step output)\n"
				<< "w(orking memory) - show contents of working memory\n"
	//			<< "o(bjects) - show information for visual, auditory, and task objects\n"
	//			<< "p(processor status) - show busy/free status of processors\n"
				<< "s(how) - info given by next letter:\n"
				<< "   n(ext step), w(m tag store), o(bject store), v(isual), a(uditory), t(ask), l(tm)\n"
	//			<< "S(tatic state) - show contents of static processors\n"
				<< "? - this list"
				<< endl;
			break;
		}
	}   // end of try block
	catch (Glean_user_error& x)
		{
			Normal_out << x.what() << endl;
			Normal_out << "Execution continuing" << endl;
		}
	}	// end of while(do_commands) loop
	
	
	// run the simulation for one cycle, then set the UI state for the next cycle
	continue_running = model_ptr->run_time(Single_cycle_duration_c);
	if(!continue_running)
		return false;
	
	// set command modes for next cycle
	if(stop_after_steps) {
		if (steps_to_stop > 0)
			steps_to_stop--;
		else {
			single_step = true;
			stop_after_steps = false;
			set_output_settings();	// reset model to last saved values
			}
		}

	if (stop_at_time)
		if (model_ptr->get_time() >= time_to_stop) {
			single_step = true;
			stop_at_time = false;
			set_output_settings();	// reset model to last saved values
			Normal_out << "Stopping after time " << time_to_stop << endl;
//			print_current_goal_and_step_names();
			}
	// if the next step has a break, go into single-step mode
	if(model_ptr->was_operator_executed() && model_ptr->is_next_step_break_set()) {
			single_step = true;
			stop_at_time = false;
			set_output_settings();	// reset model to last saved values
			Normal_out << "Break" << endl;
		}
/*	
	// if the last step was a break, go into single-step mode
	if(model_ptr->was_break_step_executed()) {
			single_step = true;
			stop_at_time = false;
			set_output_settings();	// reset model to last saved values
			Normal_out << "Break" << endl;
		}
*/
	// if single step, don't go into command mode unless something significant happened.	
	if(single_step) {
		if(model_ptr->was_operator_executed()) {
			// have to reset this flag here because we can't guarantee that the cognitive
			// processor will get a step message on the next run_duration that would reset it
			// if nothing happened. A consequence of not having this UI code down inside the
			// GOMS method execution function.
			// do we need the line below - it is so non-elegant!
			//model_ptr->reset_operator_executed();
			do_commands = true;
			}
//		else
//			do_commands = false;
		}

	return continue_running;	// return true if run can continue
}	


void CLGleanApp::set_break(bool state)
{	
	char command;
	cin >> command;
	switch (command) {
	case 'n': {		// set break on next step in main thread
		model_ptr->set_main_next_step_break_state(state);
		// for testing purposes
		state = model_ptr->get_main_next_step_break_state();
		Normal_out << "Break " << (state ? "set" : "reset") << " at next main thread step" << endl;
		break;
		}
	case 's': {		// set break on named step	
		Normal_out << "Enter break point name as: \"<goal action> <goal object> <step label>\":";
		string goal_action, goal_object, step_label;
		cin >> goal_action >> goal_object >> step_label;
		model_ptr->set_break_state(goal_action, goal_object, step_label, state);
		// for testing purposes
		state = model_ptr->get_break_state(goal_action, goal_object, step_label);
		Normal_out << "Break " << (state ? "set" : "reset") << " at \"" 
			<< goal_action << ' ' << goal_object << ": Step " << step_label << endl;
		break;
		}
	case 'f': {		// set break on step in file name and line number	
		Normal_out << "Enter break point name as: <filename - no embedded spaces> <line number - an int>";
		string filename;
		int linenumber;
		cin >> filename >> linenumber;
		if(cin.fail()) {
			Normal_out << "Could not read an integer; flushing rest of line" << endl;
			cin.clear();
			while (cin.get() != '\n') {}
			break;
			}
		model_ptr->set_break_state(filename, linenumber, state);
		// for testing purposes
		state = model_ptr->get_break_state(filename, linenumber);
		Normal_out << "Break " << (state ? "set" : "reset") << " in file \"" 
			<< filename << "\" on step at line " << linenumber << endl;
		break;
		}
	default:
		Normal_out << "Invalid command!" << endl;
		break;
	}
		
}

Symbol encode_step_name(const string& goal_action, const string& goal_object, const string& step_label)
{
		string stepname(goal_action);
		stepname += " ";
		stepname += goal_object;
		stepname += ": Step ";
		stepname += step_label;
		return stepname;
}

void CLGleanApp::turn_off_output()
{
	model_ptr->set_output_run_messages(false);
	model_ptr->set_output_run_details(false);
	model_ptr->set_output_run_memory_contents(false);
}

// remember output settings
void CLGleanApp::save_output_settings()
{
	output_label_only = model_ptr->get_output_run_messages();
	output_list_step = model_ptr->get_output_run_details();
	output_wm_contents = model_ptr->get_output_run_memory_contents();
}

void CLGleanApp::set_output_settings()
{
	model_ptr->set_output_run_messages(output_label_only);
	model_ptr->set_output_run_details(output_list_step);
	model_ptr->set_output_run_memory_contents(output_wm_contents);
}

void CLGleanApp::load_device()
{
	device_ptr = 0;
	do {
	cout << "Enter full path of device dylib: ";
	string device_path;
	cin >> device_path;
	load_and_create_device(device_path);
	} while (!device_ptr);
	
}

// the C++ types of the device class factories
typedef Device_base * create_device_fn_t();
typedef void destroy_device_fn_t(Device_base *);

void CLGleanApp::load_and_create_device(const std::string& fullpathname)
{
	// load the device library
	device_lib_handle = dlopen(fullpathname.c_str(), RTLD_LAZY);
	if (!device_lib_handle) {
		cerr << "Failed to load library: " << fullpathname << ' ' << dlerror() << endl;
		return;
		}
 
	// reset errors
	dlerror();
	// load the symbols
	// Even the C-style cast (to go with the C-style dlsym interface) does not suppress a pointer conversion warning (in gcc 4.0)
	// The problem is that dlsym returns a void *, which by Standard, can't be converted to a function pointer because
	// platforms may differ in the size of a data pointer vs a function pointer.
	create_device_fn_t * create_device_fn = (create_device_fn_t*) dlsym(device_lib_handle, "create_device");
	const char * dlsym_error = dlerror();
	if (dlsym_error) {
		cerr << "Cannot load symbol create_device: " << dlsym_error << endl;
		return;
		}
    
	// create an instance of the class
	device_ptr = create_device_fn();
}


void CLGleanApp::destroy_and_unload_device()
{
	// Even the C-style cast (to go with the C-style dlsym interface) does not suppress a pointer conversion warning (in gcc 4.0)
	// The problem is that dlsym returns a void *, which by Standard, can't be converted to a function pointer because
	// platforms may differ in the size of a data pointer vs a function pointer.
	destroy_device_fn_t * destroy_device_fn = (destroy_device_fn_t*) dlsym(device_lib_handle, "destroy_device");
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

