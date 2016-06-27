/*
 *  VisualViewObjects.cpp
 *
 *  Created by David Kieras on 11/17/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */

#include "VisualViewObject.h"
#include "Drawing_shape.h"
#include "Drawing_shape_pool.h"
#include "AppColorUtilities.h"
#include "Color_map.h"
#include "GLEANKernel/Standard_symbols.h"

VisualViewObject::VisualViewObject(CGPoint location_, CGSize size_, 
	const Symbol& label_, CGColorRef color_, const Symbol& shape_) :
		location(location_), size(size_), label(label_), color(color_), disappearing(false), shape_ptr(0), leader_ptr(0)
{
	shape_ptr = Drawing_shape_pool::get_instance().get_shape_ptr(shape_);
}

VisualViewObject::VisualViewObject(float x, float y, float width, float height,
	const Symbol& label_, CGColorRef color_, const Symbol& shape_) :
		label(label_), color(color_), disappearing(false), shape_ptr(0), leader_ptr(0)
{
	location.x = x;	// can't initialize a structure type member in the initializer list!
	location.y = y;
	size.width = width;
	size.height = height;
	shape_ptr = Drawing_shape_pool::get_instance().get_shape_ptr(shape_);
}

// minimal constructors for new objects with unknown properties
VisualViewObject::VisualViewObject(CGPoint location_, CGSize size_) : 
	location(location_), size(size_), label(""), color(gColor_Gray), disappearing(false), shape_ptr(0), leader_ptr(0)
{
	shape_ptr = Drawing_shape_pool::get_instance().get_shape_ptr("Unknown");
}

VisualViewObject::VisualViewObject(float x, float y, float width, float height) : 
	label(""), color(gColor_Gray), disappearing(false), shape_ptr(0), leader_ptr(0)
{
	location.x = x;	// can't initialize a structure type member in the initializer list!
	location.y = y;
	size.width = width;
	size.height = height;
	shape_ptr = Drawing_shape_pool::get_instance().get_shape_ptr("Unknown");
}


inline CGRect center_size_to_CGRect(CGPoint center, CGSize size)
{
	CGRect r = {{center.x - size.width/2, center.y - size.height/2}, size};
	return r;
}


void VisualViewObject::set_color(const Symbol& color_)
{
	color = Color_map::get_instance().get_color(color_);
}

void VisualViewObject::set_shape(const Symbol& shape_)
{
	shape_ptr = Drawing_shape_pool::get_instance().get_shape_ptr(shape_);
}

void VisualViewObject::set_leader(const Symbol& leader_name)
{
	leader_ptr = Drawing_shape_pool::get_instance().get_shape_ptr(leader_name);
}

void VisualViewObject::drawself(CGContextRef ctx, float scale) const
{
	assert(shape_ptr);
	shape_ptr->draw(ctx, this);
	// draw the leader, if present
	if(leader_ptr)
		leader_ptr->draw(ctx, this);
	
	if(disappearing)
		CGContextSetAlpha(ctx, 0.3);
	else
		CGContextSetAlpha(ctx, 1.0);

	// write the label if it is non-nil
	if(label == Nil_c)
		return;
//	CGContextSelectFont(ctx, "Geneva", 12, kCGEncodingMacRoman);
	CGContextSelectFont(ctx, "Monaco", 9./scale, kCGEncodingMacRoman);
	CGRect rect = center_size_to_CGRect(location, size);
	if(label.has_string_value())
		CGContextShowTextAtPoint(ctx, CGRectGetMinX(rect), CGRectGetMidY(rect), label.c_str(), label.length());
	else {
		// get the string representation for the value
		std::string str = label.str();
		CGContextShowTextAtPoint(ctx, CGRectGetMinX(rect), CGRectGetMidY(rect), str.c_str(), str.length());
		}
}


