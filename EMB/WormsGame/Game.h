#pragma once

#include "FWorm.h"
#include "JukeBox.h"
#include "WormsLib/WormsAudioHelp.h"
#include "WormsLib/TinyPixelNoise.h"
#include "WormsLib/Worms.h"


#define WORLD_WIDTH 1920
#define WORLD_HEIGHT 1080
#define GRID_COLUMNS 16
#define GRID_ROWS 16

#define DEAD_PARTS_UPDATE 1.0 / 12.0f
#define NUM_ENEMIES 7
#define SPAWN_OFFSET_DISTANCE 350
#define SELF_COLLISION_OFFSET 10

#define INITIAL_FOOD_AMOUNT 500

#define OTHERS_COLORS { {0xDFBB99, 0xB98704}, {0x774300, 0xB98704} }
#define PLAYER_COLORS { {0xB2DDBC, 0x72BB7A}, {0x3A6656, 0x72BB7A} }

struct GameState;
constexpr float GRID_WIDTH = WORLD_WIDTH / (GRID_COLUMNS - 1);
constexpr float GRID_HEIGHT = WORLD_HEIGHT / (GRID_ROWS - 1);

enum EDebugDisplay : unsigned
{
	EDebugDisplay_Breadcrumbs = 1u << 0,
	EDebugDisplay_BoundingBoxes = 1u << 1,
	EDebugDisplay_FoodBroadphase = 1u << 2,
	EDebugDisplay_MoveTargets = 1u << 3,
	EDebugDisplay_AI = 1u << 4,


	EDebugDisplay_Full =
		EDebugDisplay_Breadcrumbs |
		EDebugDisplay_BoundingBoxes |
		EDebugDisplay_FoodBroadphase |
		EDebugDisplay_MoveTargets |
		EDebugDisplay_AI
};

enum EGameRules : unsigned {
	EGameRules_SelfCollisionKills = 1u << 0,
	EGameRules_SelfCollisionCuts = 1u << 1,
	EGameRules_CollidingIntoWormCutsIt = 1u << 2,
	EGameRules_CollidingIntoWormsKillsYou = 1u << 3
};

struct Food
{
	FVec2 Position;
	int Value;
};

struct WormPart
{
	FWorm Worm;
	FColor Colors[2][2];
	std::vector<FVec2>& Points(){ return Worm.Points; }
	float HeadSize(){ return Worm.HeadSize(); }
};

struct GameContext
{

private:

	double _consume_sound_timeout = 0;

	double _nextPartsUpdate;
	GameState* _currentState;

	FWormsAudioHelp AudioHelp;
	JukeBox JukeBox;
	std::vector<Food>* FoodGridPtrs[GRID_COLUMNS * GRID_ROWS];

	std::vector<FWorm> Worms{};
	std::vector<WormPart> LostWormParts;

	std::vector <FWormAISensor> AISensors{};
	std::vector<std::unique_ptr<IWormAI>> AIs;

	std::unique_ptr<IWormAI> SharedAI;

	void Render(const FVideo& Video);
	
	bool ProcessSenseDirection(const FVec2& senseDirection, const int wormIndex, FSenseResult& result);
	bool RaycastWalls(const FVec2& senseDirection, const FVec2& rayStart, const FVec2& rayEndPoint, FSenseResult& out);
	bool RaycastOtherWorms(const FVec2& senseDirection, const int wormIndex, const FVec2& rayStart, const FVec2& rayEndPoint, FSenseResult& out);
	bool RaycastSelf(const FVec2& senseDirection, const int wormIndex, const FVec2& rayStart, const FVec2& rayEndPoint, FSenseResult& out);
	bool RaycastFood(const FVec2& senseDirection, const FVec2& rayStart, const FVec2& rayEndPoint, float cast_width, FSenseResult& out);

	std::vector<Food>& GetFoodCell(FVec2 position);
	unsigned GetAdjacentFoodCells(FVec2 position, unsigned cells[9]);
	inline bool IsPlayerWorm(FWorm* worm) { return worm == &PlayerWorm(); }

public:

	const FRect WorldBounds;
	FViewportTransform ViewPortTransform;
	unsigned DebugDisplay = 0;
	unsigned GameRules;

	GameContext(const FTime& Time);
	void ChangeState(GameState* state, const FTime& Time);
	bool ProcessDamagedWorms(const FTime& Time);
	bool Update(const FVideo& Video, const FAudio& Audio, const FInput& Input, const FTime& Time);
	void UpdateAI(const FTime& Time);
	void ProcessOverlaps(const FTime& Time);
	void ProcessDeadParts(const FTime& Time);
	void LoopGameTheme(const FTime& Time);
	void LoopGameOverTheme(const FTime& Time);
	void PlayOpeningCountdown(const FTime& Time);

	inline FWorm& PlayerWorm() {return Worms[0]; }
	
	~GameContext();
};

extern GameContext* Game;
