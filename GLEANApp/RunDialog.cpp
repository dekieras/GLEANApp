/*
 *  RunDialog.cpp
 *  EPICX
 *
 *  Created by David Kieras on 1/18/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "RunDialog.h"
#include "AppClassesUtilities.h"
#include "GleanApp.h"
#include "GLEANKernel/Model.h"
#include "GLEANKernel/Output_tee_globals.h"
#include "GLEANKernel/Glean_exceptions.h"
#include <string>
#include <iostream>

using std::string;
using std::cout;	using std::endl;

// Display dialog control constants
static const ControlID kRdia_run_continuously = {'Rdia', 100};
static const ControlID kRdia_sim_sec_per_step = {'Rdia', 101};
static const ControlID kRdia_real_sec_per_step = {'Rdia', 102};
static const ControlID kRdia_run_for = {'Rdia', 103};
static const ControlID kRdia_run_until = {'Rdia', 104};
static const ControlID kRdia_refresh_after_every = {'Rdia', 105};
static const ControlID kRdia_device_parameter_string = {'Rdia', 106};
static const ControlID kRdia_duration_group = {'Rdia', 111};
enum Duration_group_e {RUN_FOR_TIME = 1, RUN_UNTIL_TIME = 2, RUN_UNTIL_DONE = 3};
static const ControlID kRdia_refresh_group = {'Rdia', 112};
enum Refresh_group_e {EACH_STEP = 1, AFTER_EVERY_TIME = 2, NONE_DURING_RUN = 3};


RunDialog::RunDialog() :
	AppDialogBase(CFSTR("GLEANApp"), CFSTR("RunDialog"))
{
	AppDialogBase::initialize_dialog(this);
}

void RunDialog::set_initial_dialog_data()
{
	set_checkbox_value(kRdia_run_continuously,
		GleanApp::get_instance().get_run_continuously());
	set_double_value(kRdia_sim_sec_per_step, CFSTR("%6.3lf"),
		(GleanApp::get_instance().get_sim_time_per_step() / 1000.));
	set_double_value(kRdia_real_sec_per_step, CFSTR("%lf"),
		GleanApp::get_instance().get_real_time_per_step());
	set_double_value(kRdia_run_for, CFSTR("%10.3lf"),
		(GleanApp::get_instance().get_run_for_time() / 1000.));
	set_double_value(kRdia_run_until, CFSTR("%10.3lf"),
		(GleanApp::get_instance().get_run_until_time() / 1000.));
	set_double_value(kRdia_refresh_after_every, CFSTR("%7.3lf"),
		(GleanApp::get_instance().get_time_per_refresh() / 1000.));
	// don't require indentical enum values!
	GleanApp::Run_duration_e run_duration_setting = GleanApp::get_instance().get_run_duration_setting();
	switch(run_duration_setting) {
		case GleanApp::RUN_UNTIL_TIME:
			set_radiogroup_number(kRdia_duration_group, RUN_UNTIL_TIME);
			break;
		case GleanApp::RUN_UNTIL_DONE:
			set_radiogroup_number(kRdia_duration_group, RUN_UNTIL_DONE);
			break;
		default:
		case GleanApp::RUN_FOR_TIME:
			set_radiogroup_number(kRdia_duration_group, RUN_FOR_TIME);
			break;
		}
	GleanApp::Refresh_e refresh_setting = GleanApp::get_instance().get_refresh_setting();
	switch(refresh_setting) {
		case GleanApp::AFTER_EVERY_TIME:
			set_radiogroup_number(kRdia_refresh_group, AFTER_EVERY_TIME);
			break;
		case GleanApp::NONE_DURING_RUN:
			set_radiogroup_number(kRdia_refresh_group, NONE_DURING_RUN);
			break;
		default:
		case GleanApp::EACH_STEP:
			set_radiogroup_number(kRdia_refresh_group, EACH_STEP);
			break;
		}
	Model * model_ptr = GleanApp::get_instance().get_model_ptr();
	old_device_param_str = model_ptr->get_device_parameter_string();
	set_std_string_value(kRdia_device_parameter_string, old_device_param_str);
}

void RunDialog::read_dialog_data()
{
	bool run_continuously = get_checkbox_value(kRdia_run_continuously);
	if(run_continuously != GleanApp::get_instance().get_run_continuously())
		GleanApp::get_instance().set_run_continuously(run_continuously);

	long sim_sec_per_step = long(get_double_value(kRdia_sim_sec_per_step) * 1000);
	if(sim_sec_per_step != GleanApp::get_instance().get_sim_time_per_step())
		GleanApp::get_instance().set_sim_time_per_step(sim_sec_per_step);

	double real_sec_per_step = get_double_value(kRdia_real_sec_per_step);
	if(real_sec_per_step != GleanApp::get_instance().get_real_time_per_step())
		GleanApp::get_instance().set_real_time_per_step(real_sec_per_step);

	long run_for_time = long(get_double_value(kRdia_run_for) * 1000);
	if(run_for_time != GleanApp::get_instance().get_run_for_time())
		GleanApp::get_instance().set_run_for_time(run_for_time);

	long run_until_time = long(get_double_value(kRdia_run_until) * 1000);
	if(run_until_time != GleanApp::get_instance().get_run_until_time())
		GleanApp::get_instance().set_run_until_time(run_until_time);

	long time_per_refresh = long(get_double_value(kRdia_refresh_after_every) * 1000);
	if(time_per_refresh != GleanApp::get_instance().get_time_per_refresh())
		GleanApp::get_instance().set_time_per_refresh(time_per_refresh);
	// don't require indentical enum values!
	Duration_group_e run_duration_setting = Duration_group_e(get_radiogroup_number(kRdia_duration_group));
	switch(run_duration_setting) {
		case RUN_UNTIL_TIME:
			GleanApp::get_instance().set_run_duration_setting(GleanApp::RUN_UNTIL_TIME);
			break;
		case RUN_UNTIL_DONE:
			GleanApp::get_instance().set_run_duration_setting(GleanApp::RUN_UNTIL_DONE);
			break;
		default:
		case RUN_FOR_TIME:
			GleanApp::get_instance().set_run_duration_setting(GleanApp::RUN_FOR_TIME);
			break;
		}
	Refresh_group_e refresh_setting = Refresh_group_e(get_radiogroup_number(kRdia_refresh_group));
	switch(refresh_setting) {
		case AFTER_EVERY_TIME:
			GleanApp::get_instance().set_refresh_setting(GleanApp::AFTER_EVERY_TIME);
			break;
		case NONE_DURING_RUN:
			GleanApp::get_instance().set_refresh_setting(GleanApp::NONE_DURING_RUN);
			break;
		default:
		case EACH_STEP:
			GleanApp::get_instance().set_refresh_setting(GleanApp::EACH_STEP);
			break;
		}
	try {
	Model * model_ptr = GleanApp::get_instance().get_model_ptr();
	string std_str = get_std_string_value(kRdia_device_parameter_string);
	model_ptr->set_device_parameter_string(std_str);
	} catch (Exception& x) {
		Normal_out << "Error:" << x.what() << endl;
		}
}

