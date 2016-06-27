/*
 *  LogDialog.h
 *  EPICX
 *
 *  Created by David Kieras on 2/3/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef LOGDIALOG_H
#define LOGDIALOG_H
#include <Carbon/Carbon.h>
#include "AppDialogBase.h"


class LogDialog : public AppDialogBase {
public:
	LogDialog();

protected:
	virtual void set_initial_dialog_data();
	virtual void read_dialog_data();
	virtual OSStatus handle_command(HICommand command);	// called if command is not OK or cancel
};






#endif


