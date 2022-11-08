#pragma once
#include "Worms.h"



struct FWorm : public IWorm
{
	explicit FWorm(const FVec2& HeadPosition)
	{
		for (int i = 31; i >= 0; --i)
		{
			Points.push_back(HeadPosition + FVec2{ -SegmentLength * i, 0.0f });
		}
	}

	std::vector<FVec2> Points;

private:
	FVec2 HeadPosImpl() const override
	{
		return Points.back();
	}


	float HeadSizeImpl() const override
	{
		return 32.0f;
	}


	FVec2 FacingDirectionImpl() const override
	{
		return Normalize(Points.back() - Points[Points.size() - 2]);
	}

	int LengthImpl() const override
	{
		return static_cast<int>(Points.size());
	}

	void MoveTowardsImpl(const FTime& Time, const FVec2& Pos, bool Boost) override
	{
		// TODO: The worm is stretched out when boosting, which looks odd.
		// TODO: Smoother movement.
		// TODO: Speed and turning speed is not that satisfying.

		ElapsedTime += Time.DeltaTime;
		if (ElapsedTime > StepTime)
		{
			const FVec2 MoveDirection = Normalize(Pos - HeadPos(), FacingDirection());
			const FVec2 NewHeadPosition = HeadPos() + MoveDirection * SegmentLength * (Boost ? 1.5f : 1.0f);
			Points.erase(Points.begin());
			Points.push_back(NewHeadPosition);
		}

		ElapsedTime = fmod(ElapsedTime, StepTime);
	}

private:
	const float SegmentLength = 8.0f;
	const double StepTime = 1.0 / 60.0;

	double ElapsedTime = 0.0;
};