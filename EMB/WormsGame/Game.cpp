#include "GameState.h"
#include "Game.h"

#include "JukeBox.h"
#include "WormsLib/WormsVideoHelp.h"
#include "vs2019/VecMath.h"
#include <cmath>

GameContext::GameContext(const FTime* time): WorldBounds{ FRect{FVec2{-960.f, -540.f}, FVec2{960.f, 540.f}} }
                                 , AudioHelp{}
                                 , JukeBox{ time }
{
	_currentState = new IntroState;
	_currentState->SetContext(this);	

	SharedAI = FAIFactory::FirstAIFactory->MakeAI();

	Worms.push_back(FWorm({ 100, 100 }));

	//enemy count
	for (size_t i = 0; i < 3; i++)
	{
		Worms.push_back(FWorm({ 100, 150.0f + 50 * i }));
		AISensors.emplace_back();
	}

	int food_amount = 0;
	while (food_amount <= 500)
	{
		const int spawn_amount = RandomInt(1, 3);
		const float max_food_size = FWormsVideoHelp::FoodRadius(3);
		FVec2 world_safe_size = { WorldBounds.Width() - max_food_size, WorldBounds.Height() - max_food_size };

		const FVec2 position = {
			world_safe_size.X * (RandomFloat() - 0.5f),
			world_safe_size.Y * (RandomFloat() - 0.5f)
		};

		GetFoodCell(position).push_back({ position,spawn_amount });
		food_amount += spawn_amount;
	}
}


void DrawBounds(const FVideo& Video, const FVideoViewPort& ViewPort, const FViewportTransform& Transform, const FRect& rect)
{
	const FColor Color = FColor(0x991111);

	const auto ViewportWorldBoundsBegin = Transform.WorldToViewport(rect.Begin);
	const auto ViewportWorldBoundsEnd = Transform.WorldToViewport(rect.End);

	int Y = ViewPort.Y;

	for (const int YMax = std::min(std::max(0, (int)ViewportWorldBoundsBegin.Y), ViewPort.Y + ViewPort.Height); Y < YMax; ++Y)
	{
		for (int X = ViewPort.X; X < ViewPort.X + ViewPort.Width; ++X)
		{
		}
	}

	for (const int YMax = std::min(ViewPort.Y + ViewPort.Height, (int)ViewportWorldBoundsEnd.Y); Y < YMax; ++Y)
	{

		int X = ViewPort.X;

		for (const int XMax = std::min(std::max(0, (int)ViewportWorldBoundsBegin.X), ViewPort.X + ViewPort.Width); X < XMax; ++X)
		{
		}

		for (const int XMax = std::min(ViewPort.X + ViewPort.Width, (int)ViewportWorldBoundsEnd.X); X < XMax; ++X)
			Video.Buffer[X + Y * Video.Width] = Color;
	}
}

void GameContext::Render(const FVideo& Video)
{
	// Render out the game	

	FWormsVideoHelp::Grid(Video, Video.ViewPort(), ViewPortTransform, WorldBounds);

	// Let the eyes follow the mouse cursor
	//const FVec2 LookAt = ViewPortTransform.ViewportToWorld(Input.MousePos);

	if(DebugDisplay & EDebugDisplay_BoundingBoxes)
		for (auto& worm : Worms)
			DrawBounds(Video, Video.ViewPort(), ViewPortTransform, worm.Bounds);
	
	for (auto& worm : Worms)
		FWormsVideoHelp::Worm(Video, Video.ViewPort(), ViewPortTransform, IsPlayerWorm(&worm), worm.HeadSize() / 2.0f, worm.Points.size(), worm.Points.data(), nullptr);

	if (DebugDisplay & EDebugDisplay_Breadcrumbs)
		for (auto& worm : Worms)
			FWormsVideoHelp::Worm(Video, Video.ViewPort(), ViewPortTransform, &worm == &PlayerWorm() ? 0 : 1, 2, worm.BreadCrumbs.size(), worm.BreadCrumbs.data(), nullptr);


	for (size_t i = 0; i < GRID_COLUMNS * GRID_ROWS; i++)
	{
		for (auto food : FoodGrid[i])
			FWormsVideoHelp::Food(Video, Video.ViewPort(), ViewPortTransform, food.Position, food.Value);
	}	

	if (DebugDisplay & EDebugDisplay_Breadcrumbs)
	{
		unsigned food_cell_indecies[9]{};

		auto& worm = PlayerWorm();
		unsigned food_cells_count = GetAdjacentFoodCells(worm.HeadPos(), &food_cell_indecies[0]);

		for (unsigned j = 0; j < food_cells_count; j++)
		{
			auto& food_cell = FoodGrid[food_cell_indecies[j]];
			for (size_t food_index = 0; food_index < food_cell.size(); food_index++)
			{
				const auto& food = food_cell[food_index];
				const auto Size = FWormsVideoHelp::FoodRadius(food.Value) * 2.f;
				const auto Color = FColor{ 255, 0, 0 };
				Dots(Video, Video.ViewPort(), ViewPortTransform, 1, &food.Position, 0, &Size, 0, &Color, 0);
			}
		}
	}
}

void GameContext::UpdateAI(const FTime& Time)
{
	// Process all the ray casts requested from AIs		
	// Clear results from last update

	for (size_t i = 1; i < Worms.size(); i++)
	{
		SharedAI->Possess(Worms[i], AISensors[i-1]);
		auto SenseDirections = AISensors[i-1].TakeSenseDirections();

		for (auto& SenseDirection : SenseDirections)
		{
			auto RayEndPoint = Worms[i].HeadPos() + SenseDirection * 100.0f;
			auto OutsideDistance = std::min(
				std::min(WorldBounds.End.X - RayEndPoint.X, RayEndPoint.X - WorldBounds.Begin.X),
				std::min(WorldBounds.End.Y - RayEndPoint.Y, RayEndPoint.Y - WorldBounds.Begin.Y));
			if (OutsideDistance < 0.0f)
				AISensors[i - 1].AddSenseResult(FSenseResult{ ESenseHitType::Wall, Worms[i].HeadPos(), SenseDirection, 100.0f + OutsideDistance, 0 });
		}

		SharedAI->Update(Time);
	}
}

void GameContext::ChangeState(GameState *state)
{
	delete _currentState;
	_currentState = state;
	_currentState->SetContext(this);
}

void song()
{

	//Note bass[] = {
	//	{Tone_C2, 1},
	//	{Tone_F2, 1},
	//	{Tone_C2, 1},
	//	{Tone_A2, 1},
	//	{Tone_F2, 1},
	//};

	//JukeBox.PlayTune(bass, sizeof(bass) / sizeof(Note), 90);

	//Note notes[] = {
	//	{Tone_C4, 4},
	//	{Tone_B4, 4},
	//	{Tone_A4, 4},
	//	{Tone_C4, 4},
	//	{Tone_C4, 4},
	//	{Tone_D4, 4},
	//	{Tone_B4, 4},
	//	{Tone_C4, 4},
	//	{Tone_C4, 4},
	//	{Tone_C4, 4},
	//};

	//JukeBox.PlayTune(notes, sizeof(notes) / sizeof(Note), 90);
}

void GameContext::ProcessOverlaps(const FTime& Time)
{
	if(Time.DeltaTime == 0)
		return;

	if (!Inside(PlayerWorm().HeadPos(), WorldBounds))
		ChangeState(new GameOverState);

	unsigned food_cell_indecies[9]{};

	for (size_t worm_index = 0; worm_index < Worms.size(); worm_index++)
	{
		auto& worm = Worms[worm_index];
		unsigned food_cells_count = GetAdjacentFoodCells(worm.HeadPos(), &food_cell_indecies[0]);
		for (unsigned j = 0; j < food_cells_count; j++)
		{
			auto& food_cell = FoodGrid[food_cell_indecies[j]];
			for (size_t food_index = 0; food_index < food_cell.size(); food_index++)
			{
				const auto& food = food_cell[food_index];								

				const float distance = VecLength(food.Position - worm.HeadPos());
				if (distance < worm.HeadSize() * .5f + FWormsVideoHelp::FoodRadius(food.Value))
				{
					for (int k = 0; k < food.Value; k++)
						worm.Points.insert(worm.Points.begin(), worm.Points.front());

					food_cell.erase(std::next(food_cell.begin(), food_index));

					break;
				}
			}
		}
	}

	for (size_t worm_index = 0; worm_index < Worms.size(); worm_index++)
	{
		auto& worm = Worms[worm_index];

		for (size_t another_worm_index = 0; another_worm_index < Worms.size(); another_worm_index++)
		{
			if (worm_index == another_worm_index)
				continue;

			auto& another_worm = Worms[another_worm_index];		

			if(!Inside(worm.HeadPos(), another_worm.Bounds))
				continue;

			for (unsigned i = 0; i < another_worm.Points.size(); i++)
			{
				if(VecLength(worm.HeadPos() - another_worm.Points[i]) < (worm.HeadSize() + another_worm.HeadSize()) * .5f)
				{
					another_worm.DeadSegments = i + 1;
					break;
				}
			}
		}
	}

	for (size_t worm_index = 0; worm_index < Worms.size(); worm_index++)
	{
		auto& worm = Worms[worm_index];
		if(worm.DeadSegments > 0)
		{
			GetFoodCell(worm.Points.front()).push_back({ worm.Points.front(),1 });
			worm.Points.erase(worm.Points.begin());
			worm.DeadSegments--;			
		}
	}
}

std::vector<Food>& GameContext::GetFoodCell(FVec2 position)
{
	position += FVec2(WORLD_WIDTH, WORLD_HEIGHT) * .5f;
	int col = static_cast<int>(position.Y / GRID_HEIGHT);
	int row =  + static_cast<int>(position.X / GRID_WIDTH);
	return FoodGrid[col * GRID_ROWS + row];
}

unsigned GameContext::GetAdjacentFoodCells(FVec2 position, unsigned cells[9])
{
	position += FVec2(WORLD_WIDTH, WORLD_HEIGHT) * .5f;
	int col = static_cast<int>(position.Y / GRID_HEIGHT);
	int row = +static_cast<int>(position.X / GRID_WIDTH);
	int return_count = 0;

	for (int c_idnex = std::max(0, col-1); c_idnex <= std::min(col+1, GRID_COLUMNS-1); c_idnex++)
		for (int r_idnex = std::max(0, row-1); r_idnex <= std::min(row+1, GRID_ROWS-1); r_idnex++)
			cells[return_count++] = c_idnex * GRID_ROWS + r_idnex;

	return return_count;	
}


bool GameContext::Update(const FVideo& Video, const FAudio& Audio, const FInput& Input, const FTime& Time)
{
	AudioHelp.Silence(Audio);
	JukeBox.Update(AudioHelp);

	const FVec2 ViewPortCenter{ Video.Width / 2.0f, Video.Height / 2.0f };
	const float ViewPortScale = Video.Width / 1920.0f;
	FVec2 offset = ViewPortCenter - PlayerWorm().HeadPos() * ViewPortScale;
	ViewPortTransform = FViewportTransform{offset, ViewPortScale};

	_currentState->Update(Input, Time);

	AudioHelp.Mix(Audio);			
	Render(Video);
		
	const bool Continue = !Input.Escape;
	return Continue;
}


GameContext* Game;

bool GameUpdate(const FVideo& Video, const FAudio& Audio, const FInput& Input, const FTime& Time)
{
	if (Game == nullptr)
	{
		Game = new GameContext{ &Time };
		Game->DebugDisplay =
			EDebugDisplay_FoodBroadphase |
			EDebugDisplay_Breadcrumbs |
			EDebugDisplay_BoundingBoxes;
	}

	return Game->Update(Video, Audio, Input, Time);
}

