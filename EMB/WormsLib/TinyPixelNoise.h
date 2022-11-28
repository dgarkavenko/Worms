#pragma once

// Do not modify this file! See ASSIGNMENT.md.

/*
TinyPixelNoise

Copyright 2021 Jonas Kjellstrom

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal 
in the Software without restriction, including without limitation the rights 
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdint.h>
#include <math.h>
#include <float.h>
#include <type_traits>

constexpr float PI = 3.14159265358979f;
constexpr float TAU = 2.0f * PI;

struct FVec2
{
	float X, Y;
	FVec2() : X(0.0f), Y(0.0f) {}
	explicit FVec2(float V) : X(V), Y(V) {}
	FVec2(float InX, float InY) : X(InX), Y(InY) {}
	FVec2& operator += (const FVec2& V) { X += V.X; Y += V.Y; return *this; }
	FVec2& operator -= (const FVec2& V) { X -= V.X; Y -= V.Y; return *this; }
	FVec2& operator *= (const FVec2& V) { X *= V.X; Y *= V.Y; return *this; }
	FVec2& operator /= (const FVec2& V) { X /= V.X; Y /= V.Y; return *this; }
	FVec2& operator += (float V) { X += V; Y += V; return *this; }
	FVec2& operator -= (float V) { X -= V; Y -= V; return *this; }
	FVec2& operator *= (float V) { X *= V; Y *= V; return *this; }
	FVec2& operator /= (float V) { X /= V; Y /= V; return *this; }
	FVec2& operator - () { X = -X; Y = -Y; return *this; }
	float Dot(const FVec2& V) const { return X * V.X + Y * V.Y; }
	float CrossProduct(const FVec2& V) const { return X * V.Y - Y * V.X; }
	float Norm() const { return sqrtf(Dot(*this)); }
};

inline FVec2 operator * (FVec2 V, float S) { V *= S; return V; }
inline FVec2 operator / (FVec2 V, float S) { V /= S; return V; }
inline FVec2 operator + (FVec2 V, float S) { V += S; return V; }
inline FVec2 operator - (FVec2 V, float S) { V -= S; return V; }
inline FVec2 operator * (float S, FVec2 V) { V *= S; return V; }
inline FVec2 operator / (float S, FVec2 V) { V /= S; return V; }
inline FVec2 operator + (float S, FVec2 V) { V += S; return V; }
inline FVec2 operator - (float S, FVec2 V) { V -= S; return V; }
inline FVec2 operator / (FVec2 A, const FVec2& B) { A /= B; return A; }
inline FVec2 operator * (FVec2 A, const FVec2& B) { A *= B; return A; }
inline FVec2 operator + (FVec2 A, const FVec2& B) { A += B; return A; }
inline FVec2 operator - (FVec2 A, const FVec2& B) { A -= B; return A; }

inline FVec2 Normalize(const FVec2& V)
{
	const float Len = V.Norm();
	return (fabsf(Len) > FLT_EPSILON) ? V / Len : V;
}

inline FVec2 Normalize(const FVec2& V, const FVec2& VDefault)
{
	const float Len = V.Norm();
	return (fabsf(Len) > FLT_EPSILON) ? V / Len : VDefault;
}

inline FVec2 Rotate(const FVec2& V, float Degrees)
{
	const auto Radians = Degrees * PI / 180.0f;
	const auto Cos = cosf(Radians);
	const auto Sin = sinf(Radians);
	return FVec2{ V.X * Cos - V.Y * Sin, V.X * Sin + V.Y * Cos };
}

// Begin needs to be <= End or the rect will be considered degenerate.
struct FRect
{
	// Create a non-degenerate rectangle from two points (Begin <= End)
	static FRect FromPoints(const FVec2& A, const FVec2& B);

	FRect() : Begin{ 0.0f, 0.0f }, End{ 0.0f, 0.0f } {}
	FRect(const FVec2& Begin, const FVec2& End) : Begin(Begin), End(End) {}
	FVec2 Begin;
	FVec2 End;
	float Height() const { return End.Y - Begin.Y; }
	float Width() const { return End.X - Begin.X; }
	FVec2 Center() const { return (End + Begin) / 2.0f; }
	FVec2 Size() const { return End - Begin; }
};

inline FRect Grow(const FRect& R, float V)
{
	return FRect{ FVec2 { R.Begin.X - V, R.Begin.Y - V }, FVec2 { R.End.X + V, R.End.Y + V } };
}

inline bool Inside(const FVec2& Point, const FRect& Rect)
{
	return Point.X >= Rect.Begin.X && Point.Y >= Rect.Begin.Y && Point.X < Rect.End.X && Point.Y < Rect.End.Y;
}

inline bool Inside(const FRect& A, const FRect& B)
{
	// Is A inside B?
	return A.Begin.X >= B.Begin.X && A.End.X <= B.End.X &&
		A.Begin.Y >= B.Begin.Y && A.End.Y <= B.End.Y;
}

inline bool Overlap(const FRect& A, const FRect& B)
{
	const bool OutsideX = A.End.X <= B.Begin.X || A.Begin.X > B.End.X;
	const bool OutsideY = A.End.Y <= B.Begin.Y || A.Begin.Y > B.End.Y;
	return !(OutsideX || OutsideY);
}

template <typename T> T Sqr(T V) { return V * V; }


// RANDOM

extern int RandomInt(int Low, int High);
extern float RandomFloat();


// FIXED POINT

struct FFixed
{
	typedef int32_t Type;
	static const int BITS = 4;
	static const int FRACTION_MASK = (Type{1} << BITS) - 1;
	static const int INTEGER_MASK = ~FRACTION_MASK;
	static const int ONE = Type{1} << BITS;
	static const int HALF = ONE / 2;
	Type V;

	explicit FFixed(float InV) : V(static_cast<Type>(InV * ONE)) {}
	static FFixed Raw(Type InV) { return FFixed{ InV }; }
	static FFixed Zero() { return FFixed{ 0 }; }
	static FFixed One() { return FFixed{ ONE }; }
	static FFixed Half() { return FFixed{ HALF }; }
	static FFixed From(int V) { return FFixed{ static_cast<Type>(V) << BITS }; }
	FFixed operator -() const { return FFixed{ -V }; }
private:
	explicit FFixed(Type InV) : V(InV) {}
};


struct FVec2Fixed
{
	FFixed X, Y;
	explicit FVec2Fixed(FFixed V) : X(V), Y(V) {}
	explicit FVec2Fixed(FFixed InX, FFixed InY) : X(InX), Y(InY) {}
	FVec2Fixed(const FVec2& V) : X(FFixed(V.X)), Y(FFixed(V.Y)) {}
};

inline FFixed operator - (FFixed A, FFixed B) { return FFixed::Raw(A.V - B.V); }
inline FFixed operator + (FFixed A, FFixed B) { return FFixed::Raw(A.V + B.V); }
inline FFixed operator * (FFixed A, FFixed B) { return FFixed::Raw((A.V * B.V) >> FFixed::BITS); }
inline bool operator <= (FFixed A, FFixed B) { return A.V <= B.V; }
inline bool operator < (FFixed A, FFixed B) { return A.V < B.V; }
inline FFixed Min(FFixed A, FFixed B) { return A.V < B.V ? A : B; }
inline FFixed Max(FFixed A, FFixed B) { return A.V > B.V ? A : B; }
inline FFixed Floor(FFixed A) { return FFixed::Raw(A.V & FFixed::INTEGER_MASK); }
inline FFixed Center(FFixed A) { return Floor(A) + FFixed::Raw(FFixed::HALF); }

inline FVec2Fixed operator - (const FVec2Fixed& V, FFixed S) { return FVec2Fixed{ V.X - S, V.Y - S }; }
inline FVec2Fixed operator + (const FVec2Fixed& V, FFixed S) { return FVec2Fixed{ V.X + S, V.Y + S }; }
inline FVec2Fixed operator * (const FVec2Fixed& V, FFixed S) { return FVec2Fixed{ V.X * S, V.Y * S }; }
inline FVec2Fixed operator - (const FVec2Fixed& A, const FVec2Fixed& B) { return FVec2Fixed{ A.X - B.X , A.Y - B.Y }; }
inline FVec2Fixed operator + (const FVec2Fixed& A, const FVec2Fixed& B) { return FVec2Fixed{ A.X + B.X , A.Y + B.Y }; }
inline FVec2Fixed Min(const FVec2Fixed& A, const FVec2Fixed& B) { return FVec2Fixed{ Min(A.X, B.X), Min(A.Y, B.Y) }; }
inline FVec2Fixed Max(const FVec2Fixed& A, const FVec2Fixed& B) { return FVec2Fixed{ Max(A.X, B.X), Max(A.Y, B.Y) }; }
inline FFixed SquaredDistance(const FVec2Fixed& A, const FVec2Fixed& B) { return FFixed((A.X - B.X) * (A.X - B.X) + (A.Y - B.Y) * (A.Y - B.Y)); }


// VIDEO

struct FColor
{
	FColor() : Color(0) {}
	FColor(uint32_t InColor) : Color(InColor) {}
	FColor(uint8_t R, uint8_t G, uint8_t B) : Color((static_cast<uint32_t>(R) << 16) | (static_cast<uint32_t>(G) << 8) | (static_cast<uint32_t>(B) << 0)) {}
	uint8_t R() const { return static_cast<uint8_t>(Color >> 16); }
	uint8_t G() const { return static_cast<uint8_t>(Color >> 8); }
	uint8_t B() const { return static_cast<uint8_t>(Color); }
	uint8_t A() const { return static_cast<uint8_t>(Color >> 24); }
	uint32_t Color;
};

struct FVideoViewPort
{
	int X, Y;
	int Width;
	int Height;
};

struct FVideo
{
	int Width;
	int Height;
	FColor* Buffer;

	FVideoViewPort ViewPort() const
	{
		return FVideoViewPort{ 0, 0, Width, Height };
	}
};

// Scale world coordinates with Scale, then translate them with Offset
struct FViewportTransform
{
	static FViewportTransform FromOffset(FVec2 Offset) { return FViewportTransform{ Offset, 1.0f }; }

	static FViewportTransform FromOffset(float X, float Y) { return FViewportTransform{ FVec2 { X, Y }, 1.0f }; }

	FVec2 WorldToViewport(FVec2 Pos) const 
	{
		return Pos * Scale + Offset;
	}

	FVec2 ViewportToWorld(FVec2 Pos) const 
	{
		return (Pos - Offset) / Scale;
	}

	FVec2 Offset = FVec2{ 0.f, 0.f };
	float Scale = 1.0f;
};


enum class EFont
{
	Unscii8x16,
};

extern void PrintLine(
	const FVideo& Video,
	const FVideoViewPort& ViewPort,
	const FVec2& Pos,
	FColor Color,
	FColor Gradient,
	EFont Font,
	const char* Fmt,
	...);

extern void Dots(
	const FVideo& Video,
	const FVideoViewPort& ViewPort,
	const FViewportTransform& Transform,
	size_t Count,
	const FVec2* Positions,
	size_t PositionStride,
	const float* Sizes,
	size_t SizeStride,
	const FColor* Colors,
	size_t ColorStride);

extern void Circles(
	const FVideo& Video,
	const FVideoViewPort& ViewPort,
	const FViewportTransform& Transform,
	size_t Count,
	const FVec2* Positions,
	size_t PositionStride,
	const float* Sizes,
	size_t SizeStride,
	const FColor* Colors,
	size_t ColorStride,
	const float* Thicknesses,
	size_t ThicknessStride);

extern void Pixels(
	const FVideo& Video,
	const FVideoViewPort& ViewPort,
	const FViewportTransform& Transform,
	size_t Count,
	const FVec2* Positions,
	size_t PositionStride,
	const FColor* Colors,
	size_t ColorStride);



// AUDIO

struct FAudio
{
	int SampleRate;
	int SamplesToWrite;
	float* LChannel;
	float* RChannel;
};




// INPUT

struct FInput
{
	bool Left, Right, Up, Down, ActionButton0, ActionButton1, Escape;
	FVec2 MousePos;
};




// GAME UPDATE

struct FTime
{
	double DeltaTime;
	double ElapsedTime;
};

extern bool GameUpdate(const FVideo& Video, const FAudio& Audio, const FInput& Input, const FTime& Time);
