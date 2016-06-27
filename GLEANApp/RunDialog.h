/*
 *  RunDialog.h
 *  EPICX
 *
 *  Created by David Kieras on 1/19/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef RUNDIALOG_H
#define RUNDIALOG_H
#include <Carbon/Carbon.h>
#include "AppDialogBase.h"
#include <string>

class RunDialog : public AppDialogBase {
public:
	RunDialog();

protected:
	virtual void set_initial_dialog_data();
	virtual void read_dialog_data();
	
private:
	std::string old_device_param_str;
	// testing purposes
	std::string test_param_str;
};
#endif


