/*
 *  ObjectView.cpp
 *
 *  Created by David Kieras on 12/20/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */

#include "ObjectView.h"
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

ObjectView::ObjectView(CFStringRef window_name) :
	MemoryView(window_name), origin(CGPointZero), scale(1.0), old_scale(0), current_time(0)
{
	old_view_size = CGSizeZero;
	set_origin(0., 0.);
//	set_scale(10);
	
	AppDrawingWindow::initialize(this);
}

ObjectView::~ObjectView()
{
}

void ObjectView::initialize()
{
	objects.clear();
	current_time = 0;
}

void ObjectView::set_origin(float x_, float y_)
{
	origin.x = x_; 
	origin.y = y_;
	update_info();
}

void ObjectView::set_scale(float scale_)
{
	scale = scale_;
	update_info();
}

void ObjectView::update_info()
{
	ostringstream oss;
	oss << scaled_view_size.width << " X " << scaled_view_size.height 
		<< " DVA, (" << origin.x << ", " << origin.y << "), " << scale << " p/DVA";
	view_info = oss.str();
	set_changed();
}
	
void ObjectView::draw_content(CGContextRef ctx)
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
	
	// write the current origin and scale information
	CGContextSelectFont(ctx, "Monaco", scaled_font_size, kCGEncodingMacRoman);
	// prepare and write the time information
	ostringstream oss;
	oss << fixed << setprecision(2) << double(current_time) / 1000.;
	CGContextShowTextAtPoint(ctx, -xrange, yrange - line_size, oss.str().c_str(), oss.str().size());
	
//	int line_no = 2;
	float indent_size = 10; // tweak??
	CGPoint linepos;
	linepos.x = -xrange;
	linepos.y = yrange - 2 * line_size;
	
	for(objects_t::iterator obj_it = objects.begin(); obj_it != objects.end(); ++obj_it) {
		linepos.x = -xrange;
//		float line_pos = line_no * line_size;
//		CGContextSetTextPosition(ctx, -xrange, yrange - line_pos);
		CGContextSetTextPosition(ctx, linepos.x, linepos.y);
		const string& obj_name = (obj_it->first).str();
//		CGContextShowTextAtPoint(ctx, -xrange, yrange - line_pos, obj_name.c_str(), obj_name.size());
		CGContextShowText(ctx, obj_name.c_str(), obj_name.size());
//		line_no++;
		linepos = CGContextGetTextPosition(ctx);
		linepos.x = -xrange + indent_size;
		linepos.y -= line_size;
	
		for(prop_values_t::iterator prop_it = (obj_it->second).begin(); prop_it != (obj_it->second).end(); ++prop_it) {
//			float line_pos = line_no * line_size;
			const string& prop_name = (prop_it->first).str();
			const string& prop_value = (prop_it->second).str();
			CGContextSetTextPosition(ctx, linepos.x, linepos.y);
//			CGContextSetTextPosition(ctx, -xrange + indent_size, yrange - line_pos);
//			CGContextShowTextAtPoint(ctx, -xrange + indent_size, yrange - line_pos, prop_name.c_str(), prop_name.size());
			CGContextShowText(ctx, prop_name.c_str(), prop_name.size());
			//CGContextShowTextAtPoint(ctx, -xrange + 2 * indent_size +  prop_name.size(), yrange - line_pos, prop_value.c_str(), prop_value.size());
			CGContextShowText(ctx, "  ", 2); 	// Magic numbers and text!
			CGContextShowText(ctx, prop_value.c_str(), prop_value.size());
//			line_no++;
			linepos.y -= line_size;
			}
		
		}
}

/*


void CGContextSetTextPosition (
   CGContextRef c,
   CGFloat x,
   CGFloat y
);

void CGContextShowText (
   CGContextRef c,
   const char *string,
   size_t length
);



CGPoint CGContextGetTextPosition (
   CGContextRef c
);


*/

// overrides of View_base
void ObjectView::clear()
{
	initialize();
	set_changed();
}


void ObjectView::notify_memory_object_appear(const Symbol& object_name)
{			
	objects[object_name][Nil_c] = Nil_c;
	set_changed();
}

void ObjectView::notify_memory_object_erase(const Symbol& object_name)
{
	objects.erase(object_name);
	set_changed();
}

void ObjectView::notify_memory_property_erase(const Symbol& object_name, const Symbol& prop_name)
{
	objects_t::iterator it = objects.find(object_name);
	if(it != objects.end()) {
		it->second.erase(prop_name);
		set_changed();
		}
}

void ObjectView::notify_memory_property_changed(const Symbol& object_name, const Symbol& prop_name, const Symbol& prop_value)
{
	objects[object_name][prop_name] = prop_value;
	set_changed();
}

void ObjectView::notify_time(long current_time_)
{
	current_time = current_time_;
}
