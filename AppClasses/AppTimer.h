/*
 *  AppTimer.h
 *
 *  Created by David Kieras on 10/21/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */
 
 #ifndef APPTIMER_H
 #define APPTIMER_H

#include <Carbon/Carbon.h>

/*
A shallow wrapper around a Carbon Timer.

Derive from this, and override handle_timer(event)  
*/

class AppTimer {
public:
	AppTimer();
	virtual ~AppTimer();

	// supply delay and interval times in seconds; the timer is installed and
	// after the delay will start calling handle_timer_event() repeating at the interval.
	void start(double delay_, double interval_);
	// removes the timer; but can be restarted with different intervals
	void stop();
	// pauses the timer by setting its next time to forever
	void pause();
	// resumes the timer by setting its next time to the original delay
	// it will repeat at the original interval
	void resume();
	
	// override to do something when the timer fires
	virtual void handle_timer_event() = 0;
	
protected:
	// called from the derived class constructor to provide the appropriate "this" pointer.
	void initialize(AppTimer * derived_pointer);
	
private:
	AppTimer * derived_this_ptr;
	EventLoopTimerRef timer;
	EventTimerInterval delay;
	EventTimerInterval interval;
	EventLoopRef mainLoop;
	EventLoopTimerUPP timerUPP;

	// the static event handler function - when executed, calls
	// handle_timer_event() with "this" from return_derived_this_pointer().
	static void AppTimerEventHandler(EventLoopTimerRef theTimer, void* userData);
};

#endif
