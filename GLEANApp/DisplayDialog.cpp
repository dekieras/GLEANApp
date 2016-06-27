/*
 *  DisplayDialog.cpp
 *  EPICX
 *
 *  Created by David Kieras on 1/18/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "DisplayDialog.h"
#include "GleanApp.h"
#include "GLEANKernel/Model.h"

// Display dialog control constants
//static const ControlID kDdia_output_run_messages = {'Ddia', 102};
static const ControlID kDdia_output_compiler_messages = {'Ddia', 103};
static const ControlID kDdia_output_compiler_details = {'Ddia', 104};
static const ControlID kDdia_workload_report = {'Ddia', 107};
//static const ControlID kDdia_output_run_details = {'Ddia', 105};
static const ControlID kDdia_calibration_grid = {'Ddia', 106};
static const ControlID kDdia_scale = {'Ddia', 110};
static const ControlID kDdia_step_output_group = {'Ddia', 120};
enum Step_output_group_e {NONE = 1, LABELS = 2, STATEMENTS = 3};
static const ControlID kDdia_output_run_memory_contents = {'Ddia', 101};


DisplayDialog::DisplayDialog() :
	AppDialogBase(CFSTR("GLEANApp"), CFSTR("DisplayDialog"))
{
	AppDialogBase::initialize_dialog(this);
}

void DisplayDialog::set_initial_dialog_data()
{
	Model * model_ptr = GleanApp::get_instance().get_model_ptr();

	set_checkbox_value(kDdia_output_run_memory_contents, 
		model_ptr->get_output_run_memory_contents());
	set_checkbox_value(kDdia_output_compiler_messages, 
		model_ptr->get_output_compiler_messages());
	set_checkbox_value(kDdia_output_compiler_details, 
		model_ptr->get_output_compiler_details());
	set_int_value(kDdia_scale, 
		GleanApp::get_instance().get_spatialview_scale());
	set_checkbox_value(kDdia_calibration_grid, 
		GleanApp::get_instance().get_calibration_grid());
	set_checkbox_value(kDdia_workload_report, 
		GleanApp::get_instance().get_report_workload());
//	set_double_value(kMyNumberControlID, CFSTR("%5.3f"), 
//		PlayApp::get_instance().get_the_double_value());
//	set_checkbox_value(kDdia_output_run_messages, 
//		model_ptr->get_output_run_messages());
//	set_checkbox_value(kDdia_output_run_details, 
//		model_ptr->get_output_run_details());
		
	bool output_label_only = model_ptr->get_output_run_messages();
	bool output_list_step = model_ptr->get_output_run_details();
	Step_output_group_e output_value = NONE;
	if(output_label_only && !output_list_step)
		output_value = LABELS;
	else if(!output_label_only && output_list_step)
		output_value = STATEMENTS;
	set_radiogroup_number(kDdia_step_output_group, output_value);

}

void DisplayDialog::read_dialog_data()
{
	Model * model_ptr = GleanApp::get_instance().get_model_ptr();

	model_ptr->set_output_run_memory_contents(
		get_checkbox_value(kDdia_output_run_memory_contents));
	model_ptr->set_output_compiler_messages(
		get_checkbox_value(kDdia_output_compiler_messages));
	model_ptr->set_output_compiler_details(
		get_checkbox_value(kDdia_output_compiler_details));
	int scale = get_int_value(kDdia_scale);
	if(scale != GleanApp::get_instance().get_spatialview_scale())
		GleanApp::get_instance().set_spatialview_scale(scale);
	bool state = get_checkbox_value(kDdia_calibration_grid);
	if(state != GleanApp::get_instance().get_calibration_grid())
		GleanApp::get_instance().set_calibration_grid(state);
	state = get_checkbox_value(kDdia_workload_report);
	if(state != GleanApp::get_instance().get_report_workload())
		GleanApp::get_instance().set_report_workload(state);
	
	Step_output_group_e output_value = Step_output_group_e(get_radiogroup_number(kDdia_step_output_group));
	switch(output_value) {
		case NONE:
			model_ptr->set_output_run_messages(false);
			model_ptr->set_output_run_details(false);
			break;
		case LABELS:
			model_ptr->set_output_run_messages(true);
			model_ptr->set_output_run_details(false);
			break;
		default:
		case STATEMENTS:
			model_ptr->set_output_run_messages(false);
			model_ptr->set_output_run_details(true);
			break;
		}
}

