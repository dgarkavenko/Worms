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
		GameContext->ChangeState(new PlaybleState);
}

void PlaybleState::Update(const FInput& Input, const FTime& Time)
{
	GameState::Update(Input, Time);

	GameContext->UpdateAI(Time);
	GameContext->PlayerWorm().MoveTowards(Time, GameContext->ViewPortTransform.ViewportToWorld(Input.MousePos), Input.ActionButton0);
	GameContext->ProcessOverlaps(Time);
}

void GameOverState::Update(const FInput& Input, const FTime& Time)
{
	GameState::Update(Input, Time);
}
