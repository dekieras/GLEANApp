/*
 *  WmView.cpp
 *
 *  Created by David Kieras on 12/20/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */

#include "WmView.h"
#include "GLEANKernel/Standard_symbols.h"
#include "AppColorUtilities.h"
#include "Color_map.h"
#include "GLEANKernel/Utility_templates.h"
#include <iostream>
#include <iomanip>


#include <string>
#include <sstream>
#include <functional>
#include <algorithm>

using std::string;
using std::ostringstream;	using std::fixed;	using std::setprecision;
using std::list;
using std::vector;
using std::for_each;
using std::mem_fun;	using std::bind2nd;
using std::cout;	using std::endl;

WmView::WmView(CFStringRef window_name) :
	MemoryView(window_name), origin(CGPointZero), scale(1.0), old_scale(0), current_time(0)
{
	old_view_size = CGSizeZero;
	set_origin(0., 0.);
//	set_scale(10);
	
	AppDrawingWindow::initialize(this);
}

WmView::~WmView()
{
}

void WmView::initialize()
{
	tags.clear();
	object_refs.clear();
	current_time = 0;
}

void WmView::set_origin(float x_, float y_)
{
	origin.x = x_; 
	origin.y = y_;
	update_info();
}

void WmView::set_scale(float scale_)
{
	scale = scale_;
	update_info();
}

void WmView::update_info()
{
	ostringstream oss;
	oss << scaled_view_size.width << " X " << scaled_view_size.height 
		<< " DVA, (" << origin.x << ", " << origin.y << "), " << scale << " p/DVA";
	view_info = oss.str();
	set_changed();
}
	
void WmView::draw_content(CGContextRef ctx)
{	
	CGSize view_size = get_view_size(); // in untransformed coordinates
	if(!CGSizeEqualToSize(view_size, old_view_size) || scale != old_scale) {
		old_scale = scale;
		old_view_size = view_size;
		scaled_view_size.height = view_size.height / scale;
		scaled_view_size.width = view_size.width / scale;
		update_info();
		}
	// set our origin to put the (0., 0) point in the view center
	CGContextTranslateCTM(ctx, view_size.width / 2., view_size.height / 2.);
	// change the scale for drawing
	CGContextScaleCTM(ctx, scale, scale);
	// change so that the origin point is now centered
	CGContextTranslateCTM(ctx, -origin.x,  -origin.y);

	const int yrange = int(scaled_view_size.height / 2.);
	const int xrange = int(scaled_view_size.width / 2.);
	
	float font_size = 9.;
	float scaled_font_size = font_size / scale;
	float line_size = scaled_font_size;		// needs fixing for descenders
	CGContextSelectFont(ctx, "Monaco", scaled_font_size, kCGEncodingMacRoman);
	
	// prepare and write the time information
	ostringstream oss;
	oss << fixed << setprecision(2) << double(current_time) / 1000.;
	CGContextShowTextAtPoint(ctx, -xrange, yrange - line_size, oss.str().c_str(), oss.str().size());
	
	int line_no = 2;
	float indent_size = 10; // tweak??
	float line_pos = line_no * line_size;

	CGContextShowTextAtPoint(ctx, -xrange, yrange - line_pos, "Tag Store", 9);	// Magic numbers and text!
	for(tags_t::iterator tag_it = tags.begin(); tag_it != tags.end(); ++tag_it) {
		line_no++;
		line_pos = line_no * line_size;
		const string& tag_name = (tag_it->first).str();
		const string& tag_value = (tag_it->second).str();
		CGContextShowTextAtPoint(ctx, -xrange + indent_size, yrange - line_pos, tag_name.c_str(), tag_name.size());
//		CGContextShowTextAtPoint(ctx, -xrange + 2 * indent_size +  tag_name.size(), yrange - line_pos, tag_value.c_str(), tag_value.size());
		CGContextShowText(ctx, "  ", 2); 	// Magic numbers and text!
		CGContextShowText(ctx, tag_value.c_str(), tag_value.size());
		}

	line_no += 2;
	line_pos = line_no * line_size;
	CGContextShowTextAtPoint(ctx, -xrange, yrange - line_pos, "Object Store", 12);	// Magic numbers and text!
	for(object_refs_t::iterator obj_it = object_refs.begin(); obj_it != object_refs.end(); ++obj_it) {
		line_no++;
		line_pos = line_no * line_size;
		const string& obj_name = (obj_it->first).str();
		const string& proc_name = (obj_it->second).str();
		CGContextShowTextAtPoint(ctx, -xrange + indent_size, yrange - line_pos, obj_name.c_str(), obj_name.size());
//		CGContextShowTextAtPoint(ctx, -xrange + 2 * indent_size +  obj_name.size(), yrange - line_pos, proc_name.c_str(), proc_name.size());
		CGContextShowText(ctx, "  ", 2); 	// Magic numbers and text!
		CGContextShowText(ctx, proc_name.c_str(), proc_name.size());
		}
}

// overrides of View_base
void WmView::clear()
{
	initialize();
	set_changed();
}

void WmView::notify_Wm_tag_set(const Symbol& tag_name, const Symbol& tag_value)
{
	tags[tag_name] = tag_value;
	set_changed();
}

void WmView::notify_Wm_tag_erase(const Symbol& tag_name)
{
	tags.erase(tag_name);
	set_changed();
}


void WmView::notify_Wm_object_set(const Symbol& object_name, const Symbol& proc_name)
{
	object_refs[object_name] = proc_name;
	set_changed();
}

void WmView::notify_Wm_object_erase(const Symbol& object_name)
{
	object_refs.erase(object_name);
	set_changed();
}

void WmView::notify_time(long current_time_)
{
	current_time = current_time_;
}
