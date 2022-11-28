#include "WormsLib/Worms.h"

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

		std::vector<FVec2> SenseRays;
		SenseRays.emplace_back(Worm->FacingDirection());
		
		WormAISensor->Sense(SenseRays);

		// 2. Loop through and act upon the results from last update's rays
		// 
		// for (auto& Sensor : WormAISensor->SenseResults()) {...}

		bool TurnBack = false;
		for (auto& Sensor : WormAISensor->SenseResults()) 
		{
			if (Sensor.HitType == ESenseHitType::Wall)
			{
				TurnBack = true;
			}
		}

		// 3. Change the direction of the worm
		//
		// Worm->MoveTowards(...);

		auto MoveTarget = Worm->HeadPos() + Worm->FacingDirection() * (TurnBack ? -1.0f : 1.0f);
		Worm->MoveTowards(Time, MoveTarget, false);
	}
};

FAIFactoryRegistration<FWormAI> WormAIFactory("Worm AI");

