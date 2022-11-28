#pragma once
#include "WormsLib/TinyPixelNoise.h"


struct GameContext;

struct GameState
{
	virtual ~GameState() {}
	virtual void Update(const FInput& Input, const FTime& Time);
	void SetContext(GameContext* game_context) { GameContext = game_context; }
	
protected:
	GameContext* GameContext;
	float _stateTime = 0;

};


struct IntroState : public GameState
{
	void Update(const FInput& Input, const FTime& Time) override;
};

struct PlaybleState : public GameState
{
	void Update(const FInput& Input, const FTime& Time) override;
};

struct GameOverState : public GameState
{
	void Update(const FInput& Input, const FTime& Time) override;
};