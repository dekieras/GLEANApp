/*
 *  AppTimer.cpp
 *
 *  Created by David Kieras on 10/21/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */

#include "AppTimer.h"
#include "AppClassesUtilities.h"
#include <cassert>

AppTimer::AppTimer() :
	derived_this_ptr(0), timer(0), delay(kEventDurationForever), interval(kEventDurationSecond)
{
}

// called from the derived class constructor to provide the appropriate "this" pointer.
void AppTimer::initialize(AppTimer * derived_pointer)
{
	derived_this_ptr = derived_pointer;
}


AppTimer::~AppTimer()
{
	stop();
	DisposeEventLoopTimerUPP(timerUPP);
}

// call with a delay and interval in seconds to start running
void AppTimer::start(double delay_, double interval_)
{
	assert(derived_this_ptr);
//	static EventLoopTimerUPP timerUPP = NewEventLoopTimerUPP(AppTimer::AppTimerEventHandler);
	timerUPP = NewEventLoopTimerUPP(AppTimer::AppTimerEventHandler);
	delay = delay_ * kEventDurationSecond;
	interval = interval_ * kEventDurationSecond;
	stop();	// in case it is still running
	mainLoop = GetMainEventLoop();
	InstallEventLoopTimer(mainLoop, delay, interval, timerUPP, (void *)(derived_this_ptr), &timer);
}

void AppTimer::stop()
{
	if(timer) {
		RemoveEventLoopTimer (timer);
		timer = 0;
		}
}

void AppTimer::pause()
{
	SetEventLoopTimerNextFireTime (timer, kEventDurationForever);
}

// resume after the last delay and continue at the last interval
void AppTimer::resume()
{
	SetEventLoopTimerNextFireTime (timer, delay);
}

void AppTimer::AppTimerEventHandler(EventLoopTimerRef theTimer, void* userData)
{
	AppTimer * the_timer = static_cast<AppTimer *>(userData);
	the_timer->handle_timer_event();
}

/* Documentation references:
	OSStatus SetEventLoopTimerNextFireTime (EventLoopTimerRef inTimer, EventTimerInterval inNextFire);
	OSStatus RemoveEventLoopTimer (EventLoopTimerRef inTimer);	
	#define kEventDurationMillisecond  ((EventTime)(kEventDurationSecond / 1000))
	#define kEventDurationNoWait       0.0
	#define kEventDurationForever     -1.0
*/
