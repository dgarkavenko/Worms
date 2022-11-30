#include "vs2019/VecMath.h"
#include "WormsLib/Worms.h"
#include "Game.h"

#define NUM_RAYCASTS_PER_FRAME 64

void ExtraHitData(const FSenseResult &sensor, FVec2 &hit, FVec2 normal)
{
	hit = sensor.RayDir * sensor.HitDistanceFromHead + sensor.HeadPos;

	if(sensor.HitType != ESenseHitType::Wall)
		return;

	normal = { -1, 0.0f };

	if (Approx(hit.Y, Game->WorldBounds.Begin.Y))
		normal = { 0,-1 };
	else if (Approx(hit.Y, Game->WorldBounds.End.Y))
		normal = { 0.0f,1 };
	else if (Approx(hit.X, Game->WorldBounds.Begin.X))
		normal = { 1, 0.0f };
}


struct FWormAI : public IWormAI
{
	IWorm* Worm = nullptr;
	IWormAISensor* WormAISensor = nullptr;
	void PossessImpl(IWorm& InWorm, IWormAISensor& InWormAISensor) override
	{
		Worm = &InWorm;
		WormAISensor = &InWormAISensor;
	}

	void UpdateImpl(const FTime& Time) override
	{
		// 1. Send out up to 8 rays which we'll get the results from on the next update
		// 
		// std::vector<FVec2> SenseRays;
		// ...
		// WormAISensor->Sense(SenseRays);

		FVec2 forward = Worm->FacingDirection();
		std::vector<FVec2> SenseRays;

		float step = 1.0f + (int)(Time.ElapsedTime * 10) % 5;

		for (int i = 0; i < NUM_RAYCASTS_PER_FRAME; i++)
			SenseRays.emplace_back(Rotate(forward, i % 2 == 0 ? i * step : i * -step));
				
		WormAISensor->Sense(SenseRays);

		FVec2 destination = Worm->HeadPos() + forward * 100;
		auto* derived_worm = static_cast<FWorm*>(Worm);
		derived_worm;

		int food_detected = 0;

		for (const FSenseResult& sensor : WormAISensor->SenseResults()) 
		{
			FVec2 hit;
			FVec2 normal;
			ExtraHitData(sensor, hit, normal);

			if (sensor.HitType == ESenseHitType::Wall)
			{
				destination = { 0,0 };
				break;

			}

			if(sensor.HitType == ESenseHitType::Food)
			{
				if(sensor.FoodValue > food_detected)
				{
					food_detected = sensor.FoodValue;
					destination = hit;
				}
			}
		}

		Worm->MoveTowards(Time, destination, false);
	}
};

FAIFactoryRegistration<FWormAI> WormAIFactory("Worm AI");

