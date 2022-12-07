#pragma once
#include "WormsLib/TinyPixelNoise.h"

struct GameContext;

struct GameState
{
	virtual ~GameState() {}
	virtual bool Update(const FInput& Input, const FTime& Time);
	virtual void OnEnter(const FTime& Time);
	inline void SetContext(GameContext* game_context) { _gameContext = game_context; }
	
protected:
	GameContext* _gameContext {0};
	float _stateTime = 0;
};


struct IntroState : public GameState
{
	void OnEnter(const FTime& Time);
	bool Update(const FInput& Input, const FTime& Time) override;
};

struct PlaybleState : public GameState
{
	void OnEnter(const FTime& Time);
	bool Update(const FInput& Input, const FTime& Time) override;
};

struct GameOverState : public GameState
{
	void OnEnter(const FTime& Time);
	bool Update(const FInput& Input, const FTime& Time) override;
};