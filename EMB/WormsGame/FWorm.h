#pragma once
#include "WormsLib/Worms.h"

#define BOOST_PER_SEGMENT 0.25
#define MIN_SEGMENTS_COUNT_TO_BOOST 5

struct FWorm : public IWorm
{
	int DeadSegments = 0;
	float MovementSpeed[2] = {120, 220};
	float RotationSpeed[2] = { 4, 2 };

	double boost_remaining = 0;
	
	FVec2 InputTargetPosition{ 0,0 };
	FVec2 MoveDirection{ 0,0 };
	FRect Bounds;

	std::vector<FVec2> Points;
	std::vector<FVec2> BreadCrumbs;

	explicit FWorm(const FVec2& HeadPosition);
	inline int HP() { return (int)Points.size() - 2; }

private:
	FVec2 HeadPosImpl() const override;


	float HeadSizeImpl() const override;


	FVec2 FacingDirectionImpl() const override;

	int LengthImpl() const override;

	void extrapolate_remaining_segments(int i);

	void AddToBounds(FVec2 vec2);

	void ConsumeForBoost();
	void MoveTowardsImpl(const FTime& Time, const FVec2& Pos, bool boost) override;
};