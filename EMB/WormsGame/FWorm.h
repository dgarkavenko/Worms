#pragma once
#include "WormsLib/Worms.h"


struct FWorm : public IWorm
{
	int DeadSegments = 0;
	float MovementSpeed[2] = {150, 250};
	float RotationSpeed[2] = { 3, 1.5 };

	FVec2 InputTargetPosition{ 0,0 };
	FVec2 MoveDirection{ 0,0 };
	FRect Bounds;

	std::vector<FVec2> Points;
	std::vector<FVec2> BreadCrumbs;

	explicit FWorm(const FVec2& HeadPosition);

private:
	FVec2 HeadPosImpl() const override;


	float HeadSizeImpl() const override;


	FVec2 FacingDirectionImpl() const override;

	int LengthImpl() const override;

	void extrapolate_remaining_segments(int i);

	void AddToBounds(FVec2 vec2);
	void MoveTowardsImpl(const FTime& Time, const FVec2& Pos, bool Boost) override;
};