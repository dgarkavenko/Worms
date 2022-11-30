#pragma once
#include <algorithm>

#include "WormsLib/TinyPixelNoise.h"
#include <cmath>
#include <math.h>

inline float VecLength(const FVec2& vec2){
	return std::sqrt(vec2.X * vec2.X + vec2.Y * vec2.Y);
}

template <typename T> int sign(T val)
{
	return (T(0) < val) - (val < T(0));
}

inline FVec2 LerpVectorRotation(const FVec2 &from, const FVec2 &to, float t)
{
	float from_radians = atan2(from.Y, from.X);
	float to_radians = atan2(to.Y, to.X);

	float lerp_cos = (1.0f - t) * cosf(from_radians) + t * cosf(to_radians);
	float lerp_sin = (1.0f - t) * sinf(from_radians) + t * sinf(to_radians);

	FVec2 direction = FVec2(lerp_cos, lerp_sin);
	direction /= direction.Norm();

	return  direction;
}

inline FVec2 ShortestAngleInterpolation(const FVec2& from, const FVec2& to, float t)
{
	const float from_radians = atan2(from.Y, from.X);
	const float to_radians = atan2(to.Y, to.X);
	const float shortest = fmod(fmod((to_radians - from_radians), 2 * PI) + 3 * PI, 2 * PI) - PI;
	//const float result = from_radians + shortest * t;	
	const float result = from_radians + sign(shortest) * std::min(abs(shortest), t);

	return FVec2(cosf(result), sinf(result));

}

inline bool Approx(float a, float b)
{
	return abs(a - b) < 0.001f;
}

inline bool Intersects(FVec2 P0, FVec2 P1, FVec2 C, float radius)
{
	FVec2 direction = P1 - P0;
	float distance = direction.Norm();
	direction = (fabsf(distance) > FLT_EPSILON) ? direction / distance : direction;
	
	FVec2 diretcion_to_C = C - P0;
	float projection_length = direction.Dot(diretcion_to_C);

	if (projection_length < 0)
		return false;

	if (projection_length > distance)
		return false;
	
	FVec2 projection_point = P0 + direction * projection_length;

	if ((projection_point - C).Norm() > 2 * radius)
		return false;

	return true;

}