#include "vs2019/VecMath.h"
#include "WormsLib/Worms.h"
#include "Game.h"
#include <numeric>

#define NUM_RAYCASTS_PER_FRAME 8
#define NUM_DIRECTIONS 32

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
	int step = 0;
	double new_direction = 0;
	IWorm* Worm = nullptr;
	IWormAISensor* WormAISensor = nullptr;
	float DirectionAttractiveness[NUM_DIRECTIONS]={30};
	int QunatinizedDirection;

	void PossessImpl(IWorm& InWorm, IWormAISensor& InWormAISensor) override
	{
		Worm = &InWorm;
		WormAISensor = &InWormAISensor;
	}

	void UpdateImpl(const FTime& Time) override
	{
		//FVec2 forward = Worm->FacingDirection();
		FVec2 right = { -1, 0 };
		std::vector<FVec2> SenseRays;
		new_direction -= Time.DeltaTime;
		step++;

		for (int i = 0; i < NUM_RAYCASTS_PER_FRAME; i++)
			SenseRays.emplace_back(Rotate(right, (float)i * 15 + (step % 3) * 90));
				
		WormAISensor->Sense(SenseRays);

		for (unsigned i = 0; i < NUM_DIRECTIONS; i++)
		{
			float target = 30;
			int s = sign(target - DirectionAttractiveness[i]);
			DirectionAttractiveness[i] += (float)s * (float)Time.DeltaTime * 10;
		}

		
		//for (const FSenseResult& sensor : WormAISensor->SenseResults())
		//{
		//	int index = Quantanize(sensor.RayDir, NUM_DIRECTIONS);
		//	float& attractiveness = DirectionAttractiveness[index];
		//	attractiveness = 20.0f + 30.0f * sensor.FoodValue;

		//}

		for (const FSenseResult& sensor : WormAISensor->SenseResults())
		{
			int index = Quantanize(sensor.RayDir, NUM_DIRECTIONS);

			auto hitData = ExtraHitData{ sensor };

			if (sensor.HitType == ESenseHitType::Wall || sensor.HitType == ESenseHitType::Worm)
			{
				DirectionAttractiveness[(index + 16) % NUM_DIRECTIONS] = 60;
				DirectionAttractiveness[(index + 15) % NUM_DIRECTIONS] = 50;
				DirectionAttractiveness[(index + 14) % NUM_DIRECTIONS] = 50;
			}
		}

		for (const FSenseResult& sensor : WormAISensor->SenseResults()) 
		{
			int index = Quantanize(sensor.RayDir, NUM_DIRECTIONS);
						
			float& attractiveness = DirectionAttractiveness[index];
			auto hitData = ExtraHitData{ sensor };

			if (sensor.HitType == ESenseHitType::Wall || sensor.HitType == ESenseHitType::Worm)
			{
				attractiveness = 0;

				DirectionAttractiveness[(index + 2) % NUM_DIRECTIONS] = -10;
				DirectionAttractiveness[(index + 1) % NUM_DIRECTIONS] = -10;
				DirectionAttractiveness[(index - 1) % NUM_DIRECTIONS] = -10;
				DirectionAttractiveness[(index - 2) % NUM_DIRECTIONS] = -10;

			}		
		}

		if(new_direction <= 0 ||
			DirectionAttractiveness[QunatinizedDirection] < 0 ||
			DirectionAttractiveness[(QunatinizedDirection - 1) % NUM_DIRECTIONS] < 0 ||
			DirectionAttractiveness[(QunatinizedDirection + 1) % NUM_DIRECTIONS] < 0 ||
			DirectionAttractiveness[(QunatinizedDirection + 2) % NUM_DIRECTIONS] < 0 ||
			DirectionAttractiveness[(QunatinizedDirection - 2) % NUM_DIRECTIONS] < 0)
		{
			QunatinizedDirection = GetRandomIndex(DirectionAttractiveness, NUM_DIRECTIONS);
			new_direction = 1;
		}

		FVec2 destination = DirectionFronQuantanized(QunatinizedDirection, NUM_DIRECTIONS) * 400;
		Worm->MoveTowards(Time, destination, false);

		FWorm* derived_worm = static_cast<FWorm*>(Worm);
		std::copy(std::begin(DirectionAttractiveness), std::end(DirectionAttractiveness), std::begin(derived_worm->QunatinizedDirection));
	}
};

FAIFactoryRegistration<FWormAI> WormAIFactory("Worm AI");

