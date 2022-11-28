// Copyright Embark Studios AB

// Do not modify this file! See ASSIGNMENT.md.

#include "WormsVideoHelp.h"
#include <algorithm>
#include <vector>
#include <functional>
#include <cassert>
#include <cstring>

void FWormsVideoHelp::Clear(const FVideo& Video, const FVideoViewPort& ViewPort, FColor Color)
{
	for (int Y = ViewPort.Y; Y < ViewPort.Y + ViewPort.Height; ++Y)
	{
		for (int X = ViewPort.X; X < ViewPort.X + ViewPort.Width; ++X)
		{
			Video.Buffer[X + Y * Video.Width] = Color;
		}
	}
}

void FWormsVideoHelp::CenterTextX(const FVideo& Video, float Y, const char* Text)
{
	auto Len = strlen(Text);
	auto TextPos = FVec2{ Video.ViewPort().Width / 2.f - Len / 2.f * 8, Y };
	PrintLine(Video, Video.ViewPort(), FVec2{ TextPos.X + 1.0f, TextPos.Y + 1.0f }, FColor{ 0, 0, 0 }, FColor{ 0 }, EFont::Unscii8x16, Text);
	PrintLine(Video, Video.ViewPort(), TextPos, FColor{ 255, 255, 128 }, FColor{ 250, 240, 0 }, EFont::Unscii8x16, Text);
}

float FWormsVideoHelp::FoodRadius(int FoodValue)
{
	return FoodValue * 4.f;
}

void FWormsVideoHelp::Food(const FVideo& Video, const FVideoViewPort& ViewPort, const FViewportTransform& Transform, const FVec2& Center, int FoodValue)
{
	const auto Size = FWormsVideoHelp::FoodRadius(FoodValue) * 2.f;
	const auto Color = FColor { 255, 255, 0 };
	Dots(Video, ViewPort, Transform, 1, &Center, 0, &Size, 0, &Color, 0);
}

int FWormsVideoHelp::SkinCount()
{
	return 2;
}

void FWormsVideoHelp::Worm(const FVideo& Video, const FVideoViewPort& ViewPort, const FViewportTransform& Transform, int SkinIndex, float Radius, size_t PointCount, const FVec2* Points, const FVec2* LookAtPoint)
{
	if (!Points || PointCount < 2)
	{
		return;
	}

	SkinIndex = std::min(std::max(0, SkinIndex), FWormsVideoHelp::SkinCount() - 1);
	auto Color = std::vector<FColor>(PointCount);
	std::function<FColor()> Generator;
	switch (SkinIndex)
	{
	case 0: 
	{
		const FColor Colors[2][2] = {{0xDFBB99, 0xB98704}, {0x774300, 0xB98704}};
		int Index = 0;
		std::generate(rbegin(Color), rend(Color), [Colors, &Index](){
			const auto I = Index++;
			return Colors[(I / 6) % 2][I % 2]; 
		});
	} break;

	case 1:
	{
		const FColor Colors[2][2] = { {0xB2DDBC, 0x72BB7A}, {0x3A6656, 0x72BB7A} };
		int Index = 0;
		std::generate(rbegin(Color), rend(Color), [Colors, &Index]() {
			const auto I = Index++;
			return Colors[(I / 6) % 2][I % 2];
			});
	} break;

	default:
	{
		assert(0);
	} break;
	}

	
	
	const float Diameter = 2.f * Radius;

	Dots(Video, ViewPort, Transform, PointCount, Points, sizeof(Points[0]), &Diameter, 0, Color.data(), sizeof(FColor));

	const auto Pos = Points[PointCount - 1];
	auto Dir = Pos - Points[PointCount - 2];
	const auto DirNorm = Dir.Norm();
	Dir = DirNorm > 0.001f ? Dir/DirNorm : FVec2{1.0f, 0.0f};
	const auto Right = FVec2{ -Dir.Y, Dir.X };
	FVec2 Eyes[2] = {
		Pos + Dir * Radius / 2.5f + Right * Radius / 2,
		Pos + Dir * Radius / 2.5f - Right * Radius / 2
	};
	const auto EyeDiameter = Diameter / 2.5f;
	const auto EyeWhite = FColor{ 255, 255, 255 };
	Dots(Video, ViewPort, Transform, 2, Eyes, sizeof(FVec2), &EyeDiameter, 0, &EyeWhite, 0);

	auto DesiredDirection = FVec2{0.0f, 0.0f};
	if (LookAtPoint)
	{
		DesiredDirection = *LookAtPoint - Pos;
		const auto DesiredDirectionNorm = DesiredDirection.Norm();
		if (DesiredDirectionNorm > 0.001f)
		{
			DesiredDirection = DesiredDirection / DesiredDirectionNorm;
		}
	}

	const FVec2 Pupils[2] = {
		Eyes[0] + DesiredDirection * EyeDiameter / 4.0f,
		Eyes[1] + DesiredDirection * EyeDiameter / 4.0f,
	};
	const auto PupilBlack = FColor{ 0, 0, 45 };
	const auto PupilDiameter = EyeDiameter / 2.f;
	Dots(Video, ViewPort, Transform, 2, Pupils, sizeof(FVec2), &PupilDiameter, 0, &PupilBlack, 0);
}


void FWormsVideoHelp::Grid(const FVideo& Video, const FVideoViewPort& ViewPort, const FViewportTransform& Transform, const FRect& WorldBounds)
{
	const int GridX = 32 * 1000 + static_cast<int>(-Transform.Offset.X);
	const int GridY = 32 * 1000 + static_cast<int>(-Transform.Offset.Y);
	const FColor GridColors[2] = { FColor(0x315688), FColor(0x264466) };
	const auto OutOfBoundsColor = FColor(0x59595F);

	const auto ViewportWorldBoundsBegin = Transform.WorldToViewport(WorldBounds.Begin);
	const auto ViewportWorldBoundsEnd = Transform.WorldToViewport(WorldBounds.End);

	int Y = ViewPort.Y;

	for (const int YMax = std::min(std::max(0, (int)ViewportWorldBoundsBegin.Y), ViewPort.Y + ViewPort.Height); Y < YMax; ++Y)
	{
		for (int X = ViewPort.X; X < ViewPort.X + ViewPort.Width; ++X)
		{
			Video.Buffer[X + Y * Video.Width] = OutOfBoundsColor;
		}
	}

	for (const int YMax = std::min(ViewPort.Y + ViewPort.Height, (int)ViewportWorldBoundsEnd.Y); Y < YMax; ++Y)
	{
		const auto GridYOffset = ((GridY + Y) / 32) % 2;

		int X = ViewPort.X;

		for (const int XMax = std::min(std::max(0, (int)ViewportWorldBoundsBegin.X), ViewPort.X + ViewPort.Width); X < XMax; ++X)
		{
			Video.Buffer[X + Y * Video.Width] = OutOfBoundsColor;
		}

		for (const int XMax = std::min(ViewPort.X + ViewPort.Width, (int)ViewportWorldBoundsEnd.X); X < XMax; ++X)
		{
			Video.Buffer[X + Y * Video.Width] = GridColors[(GridYOffset + ((GridX + X) / 32)) % 2];
		}

		for (; X < ViewPort.X + ViewPort.Width; ++X)
		{
			Video.Buffer[X + Y * Video.Width] = OutOfBoundsColor;
		}
	}

	for (; Y < ViewPort.Y + ViewPort.Height; ++Y)
	{
		for (int X = ViewPort.X; X < ViewPort.X + ViewPort.Width; ++X)
		{
			Video.Buffer[X + Y * Video.Width] = OutOfBoundsColor;
		}
	}
}