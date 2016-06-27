/*
 *  TraceDialog.cpp
 *  EPICX
 *
 *  Created by David Kieras on 1/18/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "TraceDialog.h"
#include "GleanApp.h"
#include "GLEANKernel/Model.h"

// Display dialog control constants
static const ControlID kTdia_visual = {'Tdia', 101};
static const ControlID kTdia_auditory = {'Tdia', 102};
static const ControlID kTdia_cognitive = {'Tdia', 103};
static const ControlID kTdia_wm = {'Tdia', 104};
static const ControlID kTdia_task = {'Tdia', 105};
static const ControlID kTdia_manual = {'Tdia', 106};
static const ControlID kTdia_vocal = {'Tdia', 107};
static const ControlID kTdia_highlevel = {'Tdia', 108};
static const ControlID kTdia_device = {'Tdia', 109};


TraceDialog::TraceDialog() :
	AppDialogBase(CFSTR("GLEANApp"), CFSTR("TraceDialog"))
{
	AppDialogBase::initialize_dialog(this);
}

void TraceDialog::set_initial_dialog_data()
{
	Model * model_ptr = GleanApp::get_instance().get_model_ptr();

	set_checkbox_value(kTdia_visual, model_ptr->get_trace_visual());
	set_checkbox_value(kTdia_auditory, model_ptr->get_trace_auditory());
	set_checkbox_value(kTdia_cognitive, model_ptr->get_trace_cognitive());
	set_checkbox_value(kTdia_wm, model_ptr->get_trace_WM());
	set_checkbox_value(kTdia_task, model_ptr->get_trace_task());
	set_checkbox_value(kTdia_manual, model_ptr->get_trace_manual());
	set_checkbox_value(kTdia_vocal, model_ptr->get_trace_vocal());
	set_checkbox_value(kTdia_highlevel, model_ptr->get_trace_highlevel());
	set_checkbox_value(kTdia_device, model_ptr->get_trace_device());

}

void TraceDialog::read_dialog_data()
{
	Model * model_ptr = GleanApp::get_instance().get_model_ptr();

	model_ptr->set_trace_visual(get_checkbox_value(kTdia_visual));
	model_ptr->set_trace_auditory(get_checkbox_value(kTdia_auditory));
	model_ptr->set_trace_cognitive(get_checkbox_value(kTdia_cognitive));
	model_ptr->set_trace_WM(get_checkbox_value(kTdia_wm));
	model_ptr->set_trace_task(get_checkbox_value(kTdia_task));
	model_ptr->set_trace_manual(get_checkbox_value(kTdia_manual));
	model_ptr->set_trace_vocal(get_checkbox_value(kTdia_vocal));
	model_ptr->set_trace_highlevel(get_checkbox_value(kTdia_highlevel));
	model_ptr->set_trace_device(get_checkbox_value(kTdia_device));

	GleanApp::get_instance().handle_trace_settings();
}

