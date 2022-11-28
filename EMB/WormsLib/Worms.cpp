// Copyright Embark Studios AB

// Do not modify this file! See ASSIGNMENT.md.

#include "Worms.h"
#include "TinyPixelNoise.h"
#include <algorithm>
#include <functional>
#include <cassert>

void FWormAISensor::SenseImpl(const std::vector<FVec2>& Directions)
{	
	constexpr size_t MAX_RAYS = 8;
	SenseDirections = Directions;
	SenseDirections.resize(std::min(Directions.size(), MAX_RAYS));
}

const std::vector<FSenseResult>& FWormAISensor::SenseResultsImpl() const
{
	return SenseResults;
}

std::vector<FVec2> FWormAISensor::TakeSenseDirections()
{
	std::vector<FVec2> Result;
	Result.swap(SenseDirections);
	SenseResults.clear();
	return Result;
}

void FWormAISensor::AddSenseResult(const FSenseResult& SenseResult)
{
	SenseResults.push_back(SenseResult);
}

const FAIFactory* FAIFactory::FirstAIFactory;

std::unique_ptr<IWormAI> FAIFactory::MakeAI() const
{ 
	return MakeAIImpl(); 
}

const FAIFactory* FAIFactory::NextAIFactory() const
{
	return Next;
}

const char* FAIFactory::FriendlyAIName() const
{
	return Name;
}

FAIFactory::FAIFactory(const char* InFriendlyName)
: Name(InFriendlyName)
, Next(FAIFactory::FirstAIFactory)
{
	FAIFactory::FirstAIFactory = this;
}
