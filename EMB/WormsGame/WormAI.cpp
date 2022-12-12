#include "WormsLib/Worms.h"
#include "WormAI.h"

#include "vs2019/Logger.h"


FAIFactoryRegistration<FWormAI> WormAIFactory("Worm AI");

void FWormAI::PossessImpl(IWorm& InWorm, IWormAISensor& InWormAISensor)
{
	Worm = &InWorm;
	WormAISensor = &InWormAISensor;
}

void FWormAI::UpdateSenseDirections(const FTime& Time)
{
	std::vector<FVec2> SenseRays;
	decision_timeout -= Time.DeltaTime;

	for (int i = 0; i < NUM_RAYCASTS_PER_FRAME; i++)
		SenseRays.emplace_back(Rotate({right}, (float)(i * RAYCAST_PRECISSION + accumulated_raycast_offset)));

	accumulated_raycast_offset += NUM_RAYCASTS_PER_FRAME * RAYCAST_PRECISSION;
	accumulated_raycast_offset = accumulated_raycast_offset % 360;

	for (int i = 0; i < NUM_RAYCASTS_PER_FRAME; i++)
	{
		char log[32];
		sprintf_s(log, "[a %.2f][o %i]\n", (float)i * RAYCAST_PRECISSION + accumulated_raycast_offset, accumulated_raycast_offset);

		Logger::Output(log);

	}

	WormAISensor->Sense(SenseRays);
}

void FWormAI::UpdateImpl(const FTime& Time)
{
	UpdateSenseDirections(Time);

	//reset attractiveness over time
	for (unsigned i = 0; i < NUM_DIRECTIONS; i++)
	{
		const float delta_to_target = 0 - DirectionAttractiveness[i];
		const float change = (float)Time.DeltaTime * ATTRACTIVENESS_DETERIORATION;
		const int s = sign(delta_to_target);

		if (std::abs(delta_to_target) > change)
			DirectionAttractiveness[i] += s * change;
		else
			DirectionAttractiveness[i] = 0;
	}

	//food attractiveness
	for (const FSenseResult& sensor : WormAISensor->SenseResults())
	{
		if (sensor.HitType == ESenseHitType::Food)
		{
			const int index = Quantanize(sensor.RayDir, NUM_DIRECTIONS);
			float& attractiveness = DirectionAttractiveness[index];
			//turn speed is to slow to get food this close
			if (attractiveness >= 0 && sensor.HitDistanceFromHead > Worm->HeadSize())
				attractiveness = powf(BASE_FOOD_ATTRACTIVENESS, (float)sensor.FoodValue);
		}
	}

	//find unattractive directions
	for (const FSenseResult& sensor : WormAISensor->SenseResults()) 
	{						
		if (sensor.HitType == ESenseHitType::Wall || sensor.HitType == ESenseHitType::Worm)
		{
			const int sidechain_size = 1 + (int)((RAYCAST_DISTANCE - sensor.HitDistanceFromHead) / HAZARD_SIDECHAINING);
			const int index = Quantanize(sensor.RayDir, NUM_DIRECTIONS);

			for (int sidechain = index - sidechain_size; sidechain <= index + sidechain_size; sidechain++)
			{
				const float index_offset = (float)std::abs(index - sidechain);
				float& attractiveness = DirectionAttractiveness[SAFE_DIRECTION(sidechain)];
				float base_penalty = inverse_lerp(HAZARD_ATTRACTIVENESS_PENALTY_MIN_AT, HAZARD_ATTRACTIVENESS_PENALTY_MAX_AT, sensor.HitDistanceFromHead);
				attractiveness = std::min(MIN_ATTRACTIVENESS * base_penalty / (1.0f + index_offset), attractiveness);
			}
		}
	}

	//set opposite directions as attractive to avoid danger
	for (size_t i = 0; i < NUM_DIRECTIONS; i++)
	{
		if(DirectionAttractiveness[i] < 0)
		{
			float& attractiveness_of_opposite = DirectionAttractiveness[OPPOSITE(i)];
			if (attractiveness_of_opposite >= 0)
				attractiveness_of_opposite = std::min(MAX_ATTRACTIVENESS, ESCAPE_DIRECTION_ATTRACTIVENESS_COEF * std::abs(DirectionAttractiveness[i]));
		}
	}

	std::vector<float> positive_values;
	float median = 0;

	for (size_t i = 0; i < NUM_DIRECTIONS; i++)
		if (DirectionAttractiveness[i] > 0)
			positive_values.emplace_back(DirectionAttractiveness[i]);

	if(!positive_values.empty())
	{
		std::nth_element(positive_values.begin(), positive_values.begin() + positive_values.size() / 2, positive_values.end());
		median = positive_values[positive_values.size() / 2];
	}

	bool needs_update = decision_timeout < 0
		||	DirectionAttractiveness[QuantinizedDirection] < median
		|| (Worm->HeadPos() - Destination).Norm() < Worm->HeadSize() * 2;

	const int offset_size = 2;
	for (int offset_index = QuantinizedDirection - offset_size; offset_index <= QuantinizedDirection + offset_size; offset_index++)
	{
		if(DirectionAttractiveness[offset_index] < 0)
		{
			needs_update = true;
			break;
		}
	}

	if(needs_update){

		//for better decision making I pow2 positive values
		float squared_attractivness[NUM_DIRECTIONS];
		std::copy(DirectionAttractiveness, DirectionAttractiveness + NUM_DIRECTIONS, squared_attractivness);
		for (size_t i = 0; i < NUM_DIRECTIONS; i++)
			squared_attractivness[i] = squared_attractivness[i] > 0 ? squared_attractivness[i] * squared_attractivness[i] : 0;

		QuantinizedDirection = GetRandomIndex(squared_attractivness, NUM_DIRECTIONS);
		if(QuantinizedDirection > 0)
		{
			Destination = Worm->HeadPos() + DirectionFronQuantanized(QuantinizedDirection, NUM_DIRECTIONS) * 200;
			decision_timeout = DESTINATION_UPDATE_TIMEOUT;
		}
	}

	Worm->MoveTowards(Time, Destination, false);
}
