/*
 *  WmView.h
 *
 *  Created by David Kieras on 3/31/08.
 *  Copyright 2008 University of Michigan. All rights reserved.
 *
 */

#ifndef WMVIEW_H
#define WMVIEW_H
#include <Carbon/Carbon.h>
#include "MemoryView.h"
#include "GLEANKernel/Symbol.h"

#include <map>
#include <string>

class WmView : public MemoryView {
public:
	WmView(CFStringRef window_name);

	virtual ~WmView();

	// called from base class to redraw the window
	virtual void draw_content(CGContextRef ctx);
	
	// accessors to control the display metrics
	void set_origin(float x_, float y_);
	void set_scale(float scale_);

	// notifications from Model
	virtual void clear();
	virtual void notify_Wm_tag_set(const Symbol& tag_name, const Symbol& tag_value);
	virtual void notify_Wm_tag_erase(const Symbol& tag_name);
	virtual void notify_Wm_object_set(const Symbol& object_name, const Symbol& proc_name);
	virtual void notify_Wm_object_erase(const Symbol& object_name);
	virtual void notify_time(long current_time_);

private:
	// metric state
	CGPoint origin;	// location of lower-left in terms of object coordinates
	float scale;
	float old_scale;
	CGSize old_view_size;
	CGSize scaled_view_size;
	std::string view_info;
	long current_time;
	typedef std::map<Symbol, Symbol> tags_t;
	typedef std::map<Symbol, Symbol> object_refs_t;
	tags_t tags;
	object_refs_t object_refs;

	void initialize();
	void update_info();
};

#endif

