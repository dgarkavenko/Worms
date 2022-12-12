#pragma once
#include "vs2019/HandyUtils.h"
#include "WormsLib/Worms.h"
#include "Game.h"

#define NUM_RAYCASTS_PER_FRAME 8
#define RAYCAST_PRECISSION 10

#define NUM_DIRECTIONS 32
#define RAYCAST_DISTANCE 200.0f
#define DESTINATION_UPDATE_TIMEOUT 3.5f

#define MAX_ATTRACTIVENESS 100.0f
#define MIN_ATTRACTIVENESS -100.0f
#define ATTRACTIVENESS_DETERIORATION 30.0f
#define BASE_FOOD_ATTRACTIVENESS 6.0f

#define HAZARD_SIDECHAINING 30.0f
#define HAZARD_ATTRACTIVENESS_PENALTY_MIN_AT 210.0f
#define HAZARD_ATTRACTIVENESS_PENALTY_MAX_AT 50.0f
#define ESCAPE_DIRECTION_ATTRACTIVENESS_COEF 2.0f

#define SAFE_DIRECTION(var) \
	((var < 0) ? var + NUM_DIRECTIONS : var) % NUM_DIRECTIONS

#define LEFT(var) \
	SAFE_DIRECTION(var-1)

#define RIGHT(var) \
	SAFE_DIRECTION(var+1)

#define OPPOSITE(var) \
	SAFE_DIRECTION(var + NUM_DIRECTIONS / 2)

struct ExtraHitData
{
	FVec2 Normal{};
	FVec2 HitPoint{};

	ExtraHitData(const FSenseResult& sensor)
	{
		HitPoint = sensor.RayDir * sensor.HitDistanceFromHead + sensor.HeadPos;

		if (sensor.HitType != ESenseHitType::Wall)
			return;

		if (Approx(HitPoint.Y, Game->WorldBounds.Begin.Y))
			Normal = { 0,-1 };
		else if (Approx(HitPoint.Y, Game->WorldBounds.End.Y))
			Normal = { 0.0f,1 };
		else if (Approx(HitPoint.X, Game->WorldBounds.Begin.X))
			Normal = { 1, 0.0f };
		else
			Normal = { -1, 0.0f };
	}
};

struct FWormAI : public IWormAI
{

private:
	const FVec2 right = { -1, 0 };
	double decision_timeout = 0;
	int accumulated_raycast_offset = 0;

public:
	IWorm* Worm = nullptr;
	IWormAISensor* WormAISensor = nullptr;
	float DirectionAttractiveness[NUM_DIRECTIONS]{0};
	int QuantinizedDirection;
	FVec2 Destination = { 1, 0};

	void PossessImpl(IWorm& InWorm, IWormAISensor& InWormAISensor) override;
	void UpdateSenseDirections(const FTime& Time);
	void UpdateImpl(const FTime& Time) override;
};

