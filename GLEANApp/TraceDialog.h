/*
 *  TraceDialog.h
 *  EPICX
 *
 *  Created by David Kieras on 1/19/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef TRACEDIALOG_H
#define TRACEDIALOG_H
#include <Carbon/Carbon.h>
#include "AppDialogBase.h"

class TraceDialog : public AppDialogBase {
public:
	TraceDialog();

protected:
	virtual void set_initial_dialog_data();
	virtual void read_dialog_data();
};
#endif
