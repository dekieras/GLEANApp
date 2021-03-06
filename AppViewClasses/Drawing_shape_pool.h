/*
 *  Drawing_shape_pool.h
 *  AppClassesDev
 *
 *  Created by David Kieras on 12/20/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */

#ifndef DRAWING_SHAPE_POOL_H
#define DRAWING_SHAPE_POOL_H
#include <Carbon/Carbon.h>
#include <map>
#include "GLEANKernel/Symbol.h"


class VisualViewObject;
class Drawing_shape;

// A singleton containing a pool of DrawingShapes keyed by Shape Name, a Symbol
class Drawing_shape_pool {
public:
	static Drawing_shape_pool& get_instance();
	// get flyweight object from shape name
	Drawing_shape * get_shape_ptr(const Symbol& shape) const;
private:
	Drawing_shape_pool();
	~Drawing_shape_pool();
	typedef std::map<Symbol, Drawing_shape *> shape_map_t;
	shape_map_t shape_map;
};

#endif

