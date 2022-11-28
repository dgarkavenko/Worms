// Copyright Embark Studios AB

// Do not modify this file! See ASSIGNMENT.md.

#pragma once

#include "TinyPixelNoise.h"

// Helper functions for drawing worms, food and the world. Usage of this class is optional.
struct FWormsVideoHelp
{
	// Clear the canvas with Color
	static void Clear(const FVideo& Video, const FVideoViewPort& ViewPort, FColor Color);

	// Draw X-centered text starting at pixel coordinate Y
	static void CenterTextX(const FVideo& Video, float Y, const char* Text);
	
	// Get the radius by which a food of FoodValue would be drawn with (can be used for hit-testing, for example)
	static float FoodRadius(int FoodValue);

	// Draw a food of FoodValue
	static void Food(const FVideo& Video, const FVideoViewPort& ViewPort, const FViewportTransform& Transform, const FVec2& Center, int FoodValue);

	// How many worm skins are there?
	static int SkinCount();
	
	// Draw a worm using the specified SkinIndex = [ 0, SkinCount() ) by drawing a dot at each point in the Points[PointCount] array.
	// Radius specifies the size of the dots.
	// The head should be last in the array of points.
	// If LookAtPoint is non-null, then the worm will try to look at that point.
	static void Worm(const FVideo& Video, const FVideoViewPort& ViewPort, const FViewportTransform& Transform, int SkinIndex, float Radius, size_t PointCount, const FVec2* Points, const FVec2* LookAtPoint);
	
	// Draw a background grid and walls
	static void Grid(const FVideo& Video, const FVideoViewPort& ViewPort, const FViewportTransform& Transform, const FRect& WorldBounds);
};
