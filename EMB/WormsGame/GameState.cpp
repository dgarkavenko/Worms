#include "GameState.h"

#include "Game.h"
#include "vs2019/Logger.h"

inline void GameState::Update(const FInput& Input, const FTime& Time)
{
	if(!Input.Escape)
		_stateTime += (float)Time.DeltaTime;
}

inline void GameState::OnEnter(const FTime& Time)
{
	Time;
}


void IntroState::OnEnter(const FTime& Time)
{
	GameState::OnEnter(Time);
	_gameContext->PlayOpening(Time);
}

void IntroState::Update(const FInput& Input, const FTime& Time)
{
	GameState::Update(Input, Time);
	if (_stateTime > 2.0f)
		_gameContext->ChangeState(new PlaybleState, Time);
}

void PlaybleState::OnEnter(const FTime& Time)
{	
	GameState::OnEnter(Time);
}

void PlaybleState::Update(const FInput& Input, const FTime& Time)
{
	GameState::Update(Input, Time);

	_gameContext->UpdateAI(Time);
	_gameContext->PlayerWorm().MoveTowards(Time, _gameContext->ViewPortTransform.ViewportToWorld(Input.MousePos), Input.ActionButton0);
	_gameContext->ProcessDeadParts(Time);
	_gameContext->LoopGameTheme(Time);
	bool alive = _gameContext->ProcessOverlaps(Time);

	if (!alive)
		_gameContext->ChangeState(new GameOverState, Time);
}

void GameOverState::OnEnter(const FTime& Time)
{
	GameState::OnEnter(Time);
}

void GameOverState::Update(const FInput& Input, const FTime& Time)
{
	GameState::Update(Input, Time);
	_gameContext->UpdateAI(Time);
	_gameContext->ProcessDeadParts(Time);
	_gameContext->LoopGameOverTheme(Time);

	if(Input.ActionButton0 || Input.ActionButton1)
		_gameContext->Reset = true;
}
