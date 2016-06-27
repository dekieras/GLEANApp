/*
 *  ColorUtilities.cpp
 *  AppClassesDev
 *
 *  Created by David Kieras on 11/29/06.
 *  Copyright 2006 University of Michigan. All rights reserved.
 *
 */
/*
The colors listed have names and values from the HTML color names and values
*/
#include "AppColorUtilities.h"


// a global color space - must be initialized first
CGColorSpaceRef gColorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);

CGColorRef gColor_Aqua		= create_color(0x00FFFF);
CGColorRef gColor_Black		= create_color(0x000000);
CGColorRef gColor_Blue		= create_color(0x0000FF);
CGColorRef gColor_Brown		= create_color(0xA52A2A);
CGColorRef gColor_Chartreuse= create_color(0x7FFF00);
CGColorRef gColor_Cyan		= create_color(0x00FFFF);
CGColorRef gColor_DarkBlue	= create_color(0x00008B);
CGColorRef gColor_DarkGray	= create_color(0xA9A9A9);
CGColorRef gColor_DarkGreen	= create_color(0x006400);
CGColorRef gColor_DarkRed	= create_color(0x8B0000);
CGColorRef gColor_DarkViolet= create_color(0x9400D3);
CGColorRef gColor_Gainsboro	= create_color(0xDCDCDC);	// a very light gray
CGColorRef gColor_Green		= create_color(0x00FF00);
CGColorRef gColor_Gray		= create_color(0x808080);
CGColorRef gColor_Fuchsia	= create_color(0xFF00FF);
CGColorRef gColor_Gold		= create_color(0xFFD700);
CGColorRef gColor_GoldenRod	= create_color(0xDAA520);
CGColorRef gColor_LightBlue	= create_color(0xADD8E6);
CGColorRef gColor_LightGray	= create_color(0xD3D3D3);
CGColorRef gColor_Magenta	= create_color(0xFF00FF);
CGColorRef gColor_Maroon	= create_color(0x800000);
CGColorRef gColor_Navy		= create_color(0x000080);
CGColorRef gColor_Olive		= create_color(0x808000);
CGColorRef gColor_Pink		= create_color(0xFFC0CB);
CGColorRef gColor_Purple	= create_color(0x800080);
CGColorRef gColor_Red		= create_color(0xFF0000);
CGColorRef gColor_RoyalBlue	= create_color(0x4169E1);
CGColorRef gColor_SlateGray	= create_color(0x708090);
CGColorRef gColor_Teal		= create_color(0x008080);
CGColorRef gColor_Turquoise	= create_color(0x40E0D0);
CGColorRef gColor_Violet	= create_color(0xEE82EE);
CGColorRef gColor_White		= create_color(0xFFFFFF);
CGColorRef gColor_Yellow	= create_color(0xFFFF00);


// create and return a CGColorRef gColor_iven the R,G,B, and alpha
CGColorRef create_color(float red, float green, float blue, float alpha)
{
	float spec[4] = {red, green, blue, alpha};
	return CGColorCreate(gColorSpace, spec);
}

// create and return a CGColorRef gColor_iven the R,G,B, expressed as a single hex number
// as in web colors, and assuming an alpha of 1.0
CGColorRef create_color(long hexcolor)
{
	long rhex = (hexcolor & 0xFF0000) >> 16;
	long ghex = (hexcolor & 0x00FF00) >> 8;
	long bhex = hexcolor & 0x0000FF;
	float spec[4] = {float(rhex/255.), float(ghex/255.), float(bhex/255.), float(1.0)};
	return CGColorCreate(gColorSpace, spec);
}


