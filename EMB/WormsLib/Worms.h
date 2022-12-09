// Copyright Embark Studios AB

// Do not modify this file! See ASSIGNMENT.md.

#pragma once

#include <vector>
#include <memory>
#include "TinyPixelNoise.h"



enum class ESenseHitType
{
	Worm,
	Food,
	Wall,
};


struct FSenseResult
{
	// What we hit
	ESenseHitType HitType;

	// Start of ray
	FVec2 HeadPos;

	// Direction of ray
	FVec2 RayDir;

	// Distance to hit from HeadPos along RayDir (HitPos = HeadPos + HitDistanceFromHead * RayDir)
	float HitDistanceFromHead;
	
	// If HitType is Food, then FoodValue is how much food is in the "pile"
	int FoodValue;
};


struct IWormAISensor
{
	// Request sensing in up to 32 directions
	void Sense(const std::vector<FVec2>& Directions) { SenseImpl(Directions); }
	
	// Get the results from previous frame's Sense call
	const std::vector<FSenseResult>& SenseResults() const { return SenseResultsImpl(); }
	
	virtual ~IWormAISensor() {}
private:
	virtual void SenseImpl(const std::vector<FVec2>& Directions) = 0;
	virtual const std::vector<FSenseResult>& SenseResultsImpl() const = 0;
};


// A basic implementation of a sensor, which optionally can be used by the game.
struct FWormAISensor : public IWormAISensor
{
	// Extract the requested sense directions and clear the sense results.
	std::vector<FVec2> TakeSenseDirections();

	// Add sense results which the AI can use on the next update.
	void AddSenseResult(const FSenseResult& SenseResult);

private:
	std::vector<FVec2> SenseDirections;
	std::vector<FSenseResult> SenseResults;

private:
	void SenseImpl(const std::vector<FVec2>& Directions) override;
	const std::vector<FSenseResult>& SenseResultsImpl() const override;
};


// The API of both player and AI worms
struct IWorm
{
	// The world position of the worm's head
	FVec2 HeadPos() const { return HeadPosImpl(); }

	// The size of the head (diameter), the body should have the same width as the head
	float HeadSize() const { return HeadSizeImpl(); }

	// The direction the head is facing
	FVec2 FacingDirection() const { return FacingDirectionImpl(); }

	// The worm's length in segments (including the head)
	int Length() const { return LengthImpl(); }

	// Move the worm towards Pos. Move faster if Boost is true.
	void MoveTowards(const FTime& Time, const FVec2& Pos, bool Boost) { MoveTowardsImpl(Time, Pos, Boost); }

	virtual ~IWorm() {}

private:
	virtual FVec2 HeadPosImpl() const = 0;
	virtual float HeadSizeImpl() const = 0;
	virtual FVec2 FacingDirectionImpl() const = 0;
	virtual int LengthImpl() const = 0;
	virtual void MoveTowardsImpl(const FTime& Time, const FVec2& Pos, bool Boost) = 0;
};


// The API of the worm's AI.
// A worm's AI is not allowed to use any global functions, constants, singletons, etc.
// E.g. knowing the world boundaries is not allowed.
struct IWormAI
{
	// Called after creation to let the AI know what worm it's controlling
	// and what sensor to use to detect items in the world.
	void Possess(IWorm& Worm, IWormAISensor& WormAISensor) { PossessImpl(Worm, WormAISensor); }
	
	// Called every frame. In this call the AI should use the sensor to get
	// knowledge of its surroundings, make decisions and move the worm accordingly.
	void Update(const FTime& Time) { UpdateImpl(Time); }
	
	virtual ~IWormAI() {}
private:
	virtual void PossessImpl(IWorm& Worm, IWormAISensor& WormAISensor) = 0;
	virtual void UpdateImpl(const FTime& Time) = 0;
};


// Worm AIs must be registered with a static FAIFactoryRegistration<> instance.
// The game can then iterate through the factories starting at FAIFactory::FirstAIFactory
// and create AIs by calling MakeAI()
struct FAIFactory
{
	std::unique_ptr<IWormAI> MakeAI() const;
	const FAIFactory* NextAIFactory() const;
	const char* FriendlyAIName() const;

	static const FAIFactory* FirstAIFactory;
	
protected:
	explicit FAIFactory(const char* FriendlyName);

private:
	virtual std::unique_ptr<IWormAI> MakeAIImpl() const = 0;
	const char* Name;
	const FAIFactory* Next;
};


template <typename T>
struct FAIFactoryRegistration : public FAIFactory
{
	explicit FAIFactoryRegistration(const char* InFriendlyAIName)
	: FAIFactory(InFriendlyAIName)
	{}

private:
	std::unique_ptr<IWormAI> MakeAIImpl() const override
	{
		return std::make_unique<T>();
	}
};




