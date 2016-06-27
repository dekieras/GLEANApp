/*
 *  AppTextWindow.h
 *
 *  Created by David Kieras on 11/9/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */
 
#ifndef APPTEXTWINDOW_H
#define APPTEXTWINDOW_H

#include <Carbon/Carbon.h>
#include "AppWindowBase.h"

class AppTextWindow : public AppWindowBase {
public:
	AppTextWindow(CFStringRef window_title);
	~AppTextWindow();
	// return the total number of bytes of text
	long get_text_size() const;
	// append the C-string to the end of the text
	void append_text(const char *);
	// delete the current data
	void clear_text();
private:
	HIViewRef textView;
	TXNObject txnobj;
	HIViewRef scrollView;
};

#endif
