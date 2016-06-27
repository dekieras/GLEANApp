/*
 *  ObjectView.h
 *
 *  Created by David Kieras on 3/28/08.
 *  Copyright 2008 University of Michigan. All rights reserved.
 *
 */

#ifndef OBJECTVIEW_H
#define OBJECTVIEW_H
#include <Carbon/Carbon.h>
#include "MemoryView.h"
#include "GLEANKernel/Symbol.h"

#include <map>
#include <string>

class ObjectView : public MemoryView {
public:
	ObjectView(CFStringRef window_name);

	virtual ~ObjectView();

	// called from base class to redraw the window
	virtual void draw_content(CGContextRef ctx);
	
	// accessors to control the display metrics
	void set_origin(float x_, float y_);
	void set_scale(float scale_);

	// notifications from Model
	virtual void clear();
	virtual void notify_memory_object_appear(const Symbol&);
	virtual void notify_memory_object_erase(const Symbol&);
	virtual void notify_memory_property_erase(const Symbol&, const Symbol&);
	virtual void notify_memory_property_changed(const Symbol&, const Symbol&, const Symbol&);
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
	typedef std::map<Symbol, Symbol> prop_values_t;
	typedef std::map<Symbol, prop_values_t > objects_t; // object <property value> container
	objects_t objects;

	void initialize();
	void update_info();
};

#endif

