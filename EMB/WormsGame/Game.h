#pragma once

#include <algorithm>
#include "FWorm.h"
#include "JukeBox.h"
#include "WormsLib/WormsVideoHelp.h"
#include "WormsLib/WormsAudioHelp.h"
#include "WormsLib/TinyPixelNoise.h"
#include "WormsLib/Worms.h"

#define WORLD_WIDTH 1920
#define WORLD_HEIGHT 1080
#define GRID_COLUMNS 16
#define GRID_ROWS 16

struct GameState;
constexpr float GRID_WIDTH = WORLD_WIDTH / GRID_COLUMNS;
constexpr float GRID_HEIGHT = WORLD_HEIGHT / GRID_COLUMNS;

enum EDebugDisplay : unsigned
{
	EDebugDisplay_Breadcrumbs = 1u << 0,
	EDebugDisplay_BoundingBoxes = 1u << 1,
	EDebugDisplay_FoodBroadphase = 1u << 2
};

enum EGameRules : unsigned {
	EGameRules_CanCollideWithSelf = 1u << 0,
	EGameRules_BodyCollissionsKill = 1u << 1
};

struct Food
{
	FVec2 Position;
	int Value;
};

struct GameContext
{
	void ChangeState(GameState* state);


private:

	GameState* _currentState;
	EGameRules Rules;

	const FRect WorldBounds;
	FWormsAudioHelp AudioHelp;
	JukeBox JukeBox;
	std::vector<Food> FoodGrid[GRID_COLUMNS * GRID_ROWS];

	std::vector<FWorm> Worms{};
	std::vector<FWorm> WormParts;

	std::vector <FWormAISensor> AISensors{};

	std::unique_ptr<IWormAI> SharedAI;

	void Render(const FVideo& Video);

	std::vector<Food>& GetFoodCell(FVec2 position);
	unsigned GetAdjacentFoodCells(FVec2 position, unsigned cells[9]);
	inline bool IsPlayerWorm(FWorm* worm) { return worm == &PlayerWorm(); }

public:

	FViewportTransform ViewPortTransform;
	unsigned DebugDisplay = 0;

	GameContext(const FTime* time);
	bool Update(const FVideo& Video, const FAudio& Audio, const FInput& Input, const FTime& Time);
	void UpdateAI(const FTime& Time);
	void ProcessOverlaps(const FTime& Time);
	inline FWorm& PlayerWorm() { return Worms[0]; }

};