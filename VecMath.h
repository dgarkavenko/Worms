#pragma once
#include "WormsLib/TinyPixelNoise.h"
#include <cmath>


inline float VecLength(const FVec2& vec2){
	return std::sqrt(vec2.X * vec2.X + vec2.Y * vec2.Y);
}

inline FVec2 LerpVectorRotation(const FVec2 &from, const FVec2 &to, float t)
{
	float from_radians = atan2(from.Y, from.X);
	float to_radians = atan2(to.Y, to.X);

	auto lerp_cos = (1 - t) * cos(from_radians) + t * cos(to_radians);
	auto lerp_sin = (1 - t) * sin(from_radians) + t * sin(to_radians);

	return Normalize(FVec2(lerp_cos, lerp_sin));
}
