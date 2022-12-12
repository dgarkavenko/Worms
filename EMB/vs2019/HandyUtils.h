#pragma once
#include <algorithm>
#include "WormsLib/TinyPixelNoise.h"
#include <cmath>
#include <math.h>
#include <random>
#include <cmath>
#include <numeric>

inline float VecLength(const FVec2& vec2){
	return std::sqrt(vec2.X * vec2.X + vec2.Y * vec2.Y);
}

template <typename T> int sign(T val)
{
	return (T(0) < val) - (val < T(0));
}

template <typename T>
float inverse_lerp(const T& start, const T& end, const T& value) {
	return std::clamp((value - start) / (end - start), (T)0, (T)1);
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

inline int Quantanize(const FVec2& direction, int numDirections)
{
	float angle = std::atan2(direction.Y, direction.X);
	if (angle < 0)
		angle += 2 * PI;

	float delta = 2 * PI / numDirections;
	return (int)(angle / delta);
}

inline FVec2 DirectionFronQuantanized(const int index, int numDirections)
{
	float radians = 2 * PI / numDirections * index;
	return { std::cos(radians), std::sin(radians) };
}

//ChatGPT generated functions


// Normalize the weights in the given array so that they add up to 1.
inline void NormalizeWeights(float* weights, int count) {
	const float totalWeight = std::accumulate(weights, weights + count, 0.0f);
	std::transform(weights, weights + count, weights, [totalWeight](float w) {
		return w / totalWeight;
		});
}

inline int GetRandomIndex(const float* weights, int count) {
	// Normalize the weights.
	float normalizedWeights[256];
	std::copy(weights, weights + count, normalizedWeights);
	NormalizeWeights(normalizedWeights, count);

	// Generate a random number between 0 and 1.
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);
	const float randomNumber = (float)dis(gen);

	// Iterate through the normalized weights, keeping a running sum.
	// When the running sum is greater than the random number, return the current index.
	float sum = 0.0f;
	for (int i = 0; i < count; ++i) {
		sum += normalizedWeights[i];
		if (sum > randomNumber) {
			return i;
		}
	}

	// If we get here, something went wrong (e.g. the weights were all 0).
	return -1;
}

// Return a vector that is perpendicular to the line defined by the given start and end points and has the same length
inline FVec2 Perp(const FVec2& start, const FVec2& end)
{
	// Calculate the vector representing the line segment
	FVec2 lineVector = end - start;

	// Return a vector that is perpendicular to the line vector and has the same length
	return FVec2(-lineVector.Y, lineVector.X);
}

inline bool LineIntersectsRectangle(const FVec2& lineStart, const FVec2& lineEnd, float width, const FRect& rect)
{
	// Check if the line's start or end point is inside the rectangle
	if (Inside(lineStart, rect) || Inside(lineEnd, rect))
	{
		// In this case, the line's start or end point is inside the rectangle, so the line intersects the rectangle
		return true;
	}

	// Calculate the vertices of the rectangle
	FVec2 vertices[4] = {
	  rect.Begin,
	  { rect.Begin.X, rect.End.Y },
	  rect.End,
	  { rect.End.X, rect.Begin.Y }
	};

	// Calculate the normal of the line
	FVec2 normal = Perp(lineStart, lineEnd);

	// Calculate the offset of the line
	float offset = normal.Dot(lineStart);

	// Calculate the minimum and maximum dot products of the line's normal with each of the rectangle's vertices
	float minDot = std::numeric_limits<float>::max();
	float maxDot = std::numeric_limits<float>::min();
	for (int i = 0; i < 4; i++)
	{
		float dot = normal.Dot(vertices[i]);
		minDot = std::min(minDot, dot);
		maxDot = std::max(maxDot, dot);
	}

	// Check if the line intersects the rectangle
	return (minDot <= offset + width / 2.0f) && (maxDot >= offset - width / 2.0f);
}


inline bool LineIntersectsCircle(const FVec2& lineStart, const FVec2& lineEnd, float width, const FVec2& center, float radius)
{
	// Calculate the vector representing the line segment
	FVec2 lineVector = lineEnd - lineStart;

	// Calculate the vector from the line's start point to the circle's center
	FVec2 lineToCircleVector = center - lineStart;

	// Calculate the length of the line segment
	float lineLength = lineVector.Norm();

	// Calculate the projection of the lineToCircleVector onto the lineVector
	float projection = lineToCircleVector.Dot(lineVector) / lineLength;

	// Check if the projection is outside the bounds of the line segment
	if (projection < 0 || projection > lineLength)
	{
		// In this case, the projection is outside the bounds of the line segment, so the line does not intersect the circle
		return false;
	}

	// Calculate the closest point on the line to the circle's center
	FVec2 closestPoint = lineStart + lineVector * projection / lineLength;

	// Calculate the distance from the closest point on the line to the circle's center
	float distance = (closestPoint - center).Norm();

	// Check if the distance is less than or equal to the circle's radius plus half of the line's width
	return distance <= radius + width / 2.0f;
}