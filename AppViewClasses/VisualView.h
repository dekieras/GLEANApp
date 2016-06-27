/*
 *  VisualView.h
 *
 *  Created by David Kieras on 12/20/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */

#ifndef VISUALVIEW_H
#define VISUALVIEW_H
#include <Carbon/Carbon.h>
#include "AppDrawingWindow.h"
#include "GLEANKernel/View_base.h"	// includes Geometry Utilities
#include "GLEANKernel/Symbol.h"

#include <map>
#include <string>

class VisualViewObject;

class VisualView : public AppDrawingWindow, public View_base {
public:
	VisualView(CFStringRef window_name);

	virtual ~VisualView();

	// called from base class to redraw the window
	virtual void draw_content(CGContextRef ctx);

	// accessors to control the display metrics
	void set_origin(float x_, float y_);
	void set_scale(float scale_);
	// set calibration grid display state
	void set_grid_on(bool grid_on_) {grid_on = grid_on_;}
	
	// notifications from Model
	virtual void clear();
	virtual void notify_eye_movement(GU::Point new_eye_location);
	virtual void notify_object_appear(const Symbol& object_name, GU::Point location, GU::Size size);
	virtual void notify_object_disappear(const Symbol&);
	virtual void notify_erase_object(const Symbol&);
	virtual void notify_visual_location_changed(const Symbol& object_name, GU::Point location);
	virtual void notify_visual_size_changed(const Symbol& object_name, GU::Size size);
	virtual void notify_visual_property_changed(const Symbol&, const Symbol&, const Symbol&);
	virtual void notify_time(long current_time_);

private:
	// metric state
	CGPoint origin;	// location of lower-left in terms of object coordinates
	float scale;
	float old_scale;
	bool grid_on;	// if true, display calibration grid
	CGSize old_view_size;
	CGSize scaled_view_size;
	std::string view_info;
	long current_time;
	// pointers to always-present objects
	VisualViewObject * fov_obj_ptr;
	VisualViewObject * cal_obj_ptr;
	typedef std::map<Symbol, VisualViewObject *> objects_t;
	objects_t objects;

	
	void initialize();
	void update_info();
};

#endif

