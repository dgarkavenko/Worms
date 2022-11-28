#include "GameState.h"

#include "Game.h"

inline void GameState::Update(const FInput& Input, const FTime& Time)
{
	if(!Input.Escape)
		_stateTime += (float)Time.DeltaTime;
}


void IntroState::Update(const FInput& Input, const FTime& Time)
{
	GameState::Update(Input, Time);
	if (_stateTime > .5f)
		_gameContext->ChangeState(new PlaybleState);
}

void PlaybleState::Update(const FInput& Input, const FTime& Time)
{
	GameState::Update(Input, Time);

	_gameContext->UpdateAI(Time);
	_gameContext->PlayerWorm().MoveTowards(Time, _gameContext->ViewPortTransform.ViewportToWorld(Input.MousePos), Input.ActionButton0);
	_gameContext->ProcessOverlaps(Time);
}

void GameOverState::Update(const FInput& Input, const FTime& Time)
{
	GameState::Update(Input, Time);
}
