/*
 *  DisplayDialog.h
 *  EPICX
 *
 *  Created by David Kieras on 1/18/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef DISPLAYDIALOG_H
#define DISPLAYDIALOG_H
#include <Carbon/Carbon.h>
#include "AppDialogBase.h"

class DisplayDialog : public AppDialogBase {
public:
	DisplayDialog();

protected:
	virtual void set_initial_dialog_data();
	virtual void read_dialog_data();
};
#endif


