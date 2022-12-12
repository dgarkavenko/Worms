#include "FWorm.h"

#include "vs2019/HandyUtils.h"

FWorm::FWorm(const FVec2& HeadPosition)
{
	constexpr int num_segments = 5;

	FVec2 spawn_offset = -Normalize(HeadPosition) * HeadSize();

	for (int i = num_segments; i >= 0; --i)
		Points.push_back(HeadPosition + spawn_offset * .5f * (float)i);
}

FVec2 FWorm::HeadPosImpl() const
{
	return Points.back();
}

float FWorm::HeadSizeImpl() const
{
	return 32.0f;
}

FVec2 FWorm::FacingDirectionImpl() const
{
	return Normalize(Points.back() - Points[Points.size() - 2]);
}

int FWorm::LengthImpl() const
{
	return static_cast<int>(Points.size());
}

void FWorm::extrapolate_remaining_segments(int i)
{
	for (; i >= 0; i--)
	{
		FVec2 direction = Points[i] - Points[i + 1];
		direction = Normalize(direction);
		Points[i] = Points[i + 1] + direction * HeadSize() * 0.5f;
	}
}

void FWorm::AddToBounds(FVec2 vec2)
{
	if(Inside(vec2, Bounds))
		return;

	auto bounds = Bounds.FromPoints(Points.back(), vec2);
	bounds = Grow(bounds, HeadSize() * .5f);

	Bounds.Begin.X = std::min(bounds.Begin.X, Bounds.Begin.X);
	Bounds.Begin.Y = std::min(bounds.Begin.Y, Bounds.Begin.Y);
	Bounds.End.X = std::max(bounds.End.X, Bounds.End.X);
	Bounds.End.Y = std::max(bounds.End.Y, Bounds.End.Y);
}

void FWorm::ConsumeForBoost()
{
	if(Points.size() > MIN_SEGMENTS_COUNT_TO_BOOST)
	{
		boost_remaining = BOOST_PER_SEGMENT;
		Points.erase(Points.begin());
	}
}

void FWorm::MoveTowardsImpl(const FTime& Time, const FVec2& Pos, bool boost)
{
	if (boost)
	{
		if(boost_remaining < 0)
			ConsumeForBoost();

		boost = boost_remaining > 0;
		boost_remaining -= Time.DeltaTime;
	}

	InputTargetPosition = Pos;
	FVec2 target_direction = Normalize(Pos - HeadPos());

	MoveDirection = ShortestAngleInterpolation(MoveDirection, target_direction, RotationSpeed[boost] * (float)Time.DeltaTime);

	float speed = MovementSpeed[boost] * (float)Time.DeltaTime;
	float distance_between = HeadSize() * .5f;
		
	Points.back() += MoveDirection * speed;
	BreadCrumbs.push_back(Points.back());

	size_t back_index = BreadCrumbs.size() - 1;
				
	int i = (int)Points.size() - 2;

	float segment_distance_remain = distance_between;
	float distance_from_head_crumb = 0;

	Bounds = Bounds.FromPoints(Points.back(), Points.back());

	while (i >= 0)
	{
		if(back_index < 1)
		{
			extrapolate_remaining_segments(i);
			break;
		}

		const FVec2 next_crumb = BreadCrumbs[back_index - 1];
		const FVec2 head_crumb = BreadCrumbs[back_index];
		const FVec2 delta = next_crumb - head_crumb;
		const float distance_to_next_crumb = VecLength(delta) - distance_from_head_crumb;

		if (distance_to_next_crumb < segment_distance_remain)
		{
			back_index--;
			segment_distance_remain -= distance_to_next_crumb;
			distance_from_head_crumb = 0;
		}
		else
		{
			distance_from_head_crumb += segment_distance_remain;
			FVec2 offset = Normalize(delta) * distance_from_head_crumb;
			Points[i] = head_crumb + offset;
			segment_distance_remain = distance_between;
			AddToBounds(Points[i]);
			i--;
		}
	}

	Bounds = Grow(Bounds, HeadSize() * .5f);

	if (back_index > 1)
		BreadCrumbs.erase(BreadCrumbs.begin(), BreadCrumbs.begin() + back_index - 1);
}
