/*
 *  AppTextWindow.cpp
 *  AppTextWindowPlay
 *
 *  Created by David Kieras on 11/9/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */

#include "AppTextWindow.h"
#include "AppClassesUtilities.h"

//#include <iostream>
//using std::cout;	using std::endl;

AppTextWindow::AppTextWindow(CFStringRef window_title) :
	AppWindowBase(CFSTR("AppClasses"), CFSTR("AppTextWindow"), window_title), 
	textView(0), txnobj(0), scrollView(0)
{	
	// get and save a reference to the scroll view
	ControlID id1 = { 'ApTW', 0 }; // set with Interface Builder in the Inspector
	OSStatus err = HIViewFindByID(HIViewGetRoot(get_WindowRef()), id1, &scrollView);
	if(err)
		throw AppClassesCreationError("Could not find embedded scroll view in AppTextWindow", err);
	// get and save a reference to the text view
	ControlID id2 = { 'ApTW', 1 }; // set with Interface Builder in the Inspector
	err = HIViewFindByID(HIViewGetRoot(get_WindowRef()), id2, &textView);
	if(err)
		throw AppClassesCreationError("Could not find embedded text view in AppTextWindow", err);
	// get and save a reference to the MLTE object in the textView
	txnobj = HITextViewGetTXNObject(textView);

	// set the MLTE parameters
	/* set the MLTE object to no-wrap */
	TXNControlTag txnControlTag[1];
	TXNControlData txnControlData[1];
	txnControlTag[0] = kTXNWordWrapStateTag;
	txnControlData[0].uValue = kTXNNoAutoWrap;
	err = TXNSetTXNObjectControls(txnobj,false,1,txnControlTag,txnControlData);
	/* set the MLTE object to autoscroll to end 
	txnControlTag[0] = kTXNAutoScrollBehaviorTag;
	txnControlData[0].uValue = kTXNAutoScrollInsertionIntoView;
	err = TXNSetTXNObjectControls(txnobj,false,1,txnControlTag,txnControlData);*/
	
	/* kTXNSelectionStateTag  kTXNSelectionOn */
	
	// initialize the base class object
	initialize(this);
}

// for debugging only
AppTextWindow::~AppTextWindow()
{
//	cout << "AppTextWindow dtor" << endl;
}


long AppTextWindow::get_text_size() const
{
	return long(TXNDataSize(txnobj));
}
	

void AppTextWindow::append_text(const char * buffer)
{
	TXNSetData(txnobj, kTXNTextData, buffer, strlen(buffer), kTXNEndOffset, kTXNEndOffset);
	HIScrollViewNavigate(scrollView, kHIScrollViewScrollToBottom);
}

void AppTextWindow::clear_text()
{
	TXNSelectAll(txnobj);
	TXNClear(txnobj);
}	

