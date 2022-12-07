#include "GameState.h"

#include "Game.h"
#include "vs2019/Logger.h"

#define START_TIMEOUT 2.0f
#define AUTO_RESTART_TIMEOUT 8.0f
#define GAME_OVER_LOCK_INPUT_TIMEOUT 2.0f

inline bool GameState::Update(const FInput& Input, const FTime& Time)
{
	if(!Input.Escape)
		_stateTime += (float)Time.DeltaTime;

	return true;
}

inline void GameState::OnEnter(const FTime& Time)
{
	Time;
}


void IntroState::OnEnter(const FTime& Time)
{
	GameState::OnEnter(Time);
	_gameContext->PlayOpeningCountdown(Time);
}

bool IntroState::Update(const FInput& Input, const FTime& Time)
{
	GameState::Update(Input, Time);
	if (_stateTime > START_TIMEOUT)
		_gameContext->ChangeState(new PlaybleState, Time);

	return true;
}

void PlaybleState::OnEnter(const FTime& Time)
{	
	GameState::OnEnter(Time);
}

bool PlaybleState::Update(const FInput& Input, const FTime& Time)
{
	GameState::Update(Input, Time);

	_gameContext->UpdateAI(Time);
	_gameContext->PlayerWorm().MoveTowards(Time, _gameContext->ViewPortTransform.ViewportToWorld(Input.MousePos), Input.ActionButton0);
	_gameContext->ProcessDeadParts(Time);
	_gameContext->LoopGameTheme(Time);
	_gameContext->ProcessOverlaps(Time);

	if (!_gameContext->ProcessDamagedWorms(Time))
		_gameContext->ChangeState(new GameOverState, Time);

	return true;
}

void GameOverState::OnEnter(const FTime& Time)
{
	GameState::OnEnter(Time);
}

bool GameOverState::Update(const FInput& Input, const FTime& Time)
{
	GameState::Update(Input, Time);
	_gameContext->UpdateAI(Time);
	_gameContext->ProcessDeadParts(Time);
	_gameContext->LoopGameOverTheme(Time);

	if(_stateTime < GAME_OVER_LOCK_INPUT_TIMEOUT)
		return true;

	return !(Input.ActionButton0 || Input.ActionButton1 || _stateTime > AUTO_RESTART_TIMEOUT);

}
