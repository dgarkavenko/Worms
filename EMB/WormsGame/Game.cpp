#include "GameState.h"
#include "Game.h"
#include "iostream"
#include "JukeBox.h"
#include "WormAI.h"
#include "WormsLib/WormsVideoHelp.h"
#include "vs2019/HandyUtils.h"

GameContext::GameContext(const FTime& Time): WorldBounds{ FRect{FVec2{-960.f, -540.f}, FVec2{960.f, 540.f}} }
                                             , AudioHelp{}
                                             , JukeBox{}
{
	
	_currentState = new IntroState;
	_currentState->SetContext(this);
	_currentState->OnEnter(Time);
	
	SharedAI = FAIFactory::FirstAIFactory->MakeAI();

	FVec2 spawn_offset = { 0, -SPAWN_OFFSET_DISTANCE };
	Worms.push_back(FWorm(spawn_offset));

	//enemy count
	for (size_t i = 0; i < NUM_ENEMIES; i++)
	{
		Worms.push_back(FWorm({ Rotate(spawn_offset, (i + 1) * 360.0f / (NUM_ENEMIES + 1)) }));
		AISensors.emplace_back();
		AIs.push_back(FAIFactory::FirstAIFactory->MakeAI());
	}

	for (size_t i = 0; i < GRID_COLUMNS * GRID_ROWS; i++)
		FoodGridPtrs[i] = new std::vector<Food>();

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

		std::vector<Food>& cell = GetFoodCell(position);
		cell.push_back({ position,spawn_amount });
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
	FWormsVideoHelp::Grid(Video, Video.ViewPort(), ViewPortTransform, WorldBounds);

	if(DebugDisplay & EDebugDisplay_BoundingBoxes)
	{
		for (auto& worm : Worms)
			DrawBounds(Video, Video.ViewPort(), ViewPortTransform, worm.Bounds);
	}
	
	for (auto& worm : Worms)
		FWormsVideoHelp::Worm(Video, Video.ViewPort(), ViewPortTransform, IsPlayerWorm(&worm), worm.HeadSize() / 2.0f, worm.Points.size(), worm.Points.data(), &worm.InputTargetPosition);

	for (auto& worm_parts : WormParts)
	{
		float Size[256];
		std::fill_n(Size, 256, worm_parts.HeadSize());

		std::vector<FColor> Color(worm_parts.Points().size());

		int Index = 0;

		std::generate(std::rbegin(Color), std::rend(Color), [worm_parts, &Index]() {
			const auto I = Index++;
			return worm_parts.Colors[(I / 6) % 2][I % 2];
			});

		Dots(Video, Video.ViewPort(), ViewPortTransform, worm_parts.Points().size(), worm_parts.Points().data(), sizeof(worm_parts.Points()[0]), Size, sizeof(Size[0]), Color.data(), sizeof(Color[0]));
	}

	if (DebugDisplay & EDebugDisplay_Breadcrumbs)
	{
		for (auto& worm : Worms)
			FWormsVideoHelp::Worm(Video, Video.ViewPort(), ViewPortTransform, &worm == &PlayerWorm() ? 0 : 1, 2, worm.BreadCrumbs.size(), worm.BreadCrumbs.data(), nullptr);

	}
	
	for (size_t i = 0; i < GRID_COLUMNS * GRID_ROWS; i++)
	{
		for (auto food : *FoodGridPtrs[i])
			FWormsVideoHelp::Food(Video, Video.ViewPort(), ViewPortTransform, food.Position, food.Value);
	}

	if (DebugDisplay & EDebugDisplay_MoveTargets)
	{
		for (auto& worm : Worms)
		{
			const auto Size1 = 25.f;
			const auto Size2 = 15.f;
			const auto Color1 = FColor{ 0, 255, 255 };
			const auto Color2 = FColor{ 255, 0, 255 };
			FVec2 lerped_direction = worm.HeadPos() + worm.MoveDirection * worm.MovementSpeed[0];
			Dots(Video, Video.ViewPort(), ViewPortTransform, 1, &worm.InputTargetPosition, 0, &Size1, 0, &Color1, 0);
			Dots(Video, Video.ViewPort(), ViewPortTransform, 1, &lerped_direction, 0, &Size2, 0, &Color2, 0);
		}
	}

	if (DebugDisplay & EDebugDisplay_FoodBroadphase)
	{
		unsigned food_cell_indicies[9]{};

		auto& worm = PlayerWorm();
		unsigned food_cells_count = GetAdjacentFoodCells(worm.HeadPos(), &food_cell_indicies[0]);

		for (unsigned j = 0; j < food_cells_count; j++)
		{
			auto& food_cell = *FoodGridPtrs[food_cell_indicies[j]];
			for (size_t food_index = 0; food_index < food_cell.size(); food_index++)
			{
				const auto& food = food_cell[food_index];
				const auto Size = FWormsVideoHelp::FoodRadius(food.Value) * 2.f;
				const auto Color = FColor{ 255, 0, 0 };
				Dots(Video, Video.ViewPort(), ViewPortTransform, 1, &food.Position, 0, &Size, 0, &Color, 0);
			}
		}
	}

	
	if (DebugDisplay & EDebugDisplay_AI)
		for (unsigned i = 0; i < AIs.size(); i++)
		{
			auto ptr = AIs[i].get();
			FWormAI* derived = static_cast<FWormAI*>(ptr);

			if (derived->Worm)
				for (int j = 0; j < 32; j++)
				{
					float value = std::clamp(derived->DirectionAttractiveness[j], -100.0f, 100.0f);
					float offset = derived->QuantinizedDirection == j ? 110.0f : 100.0f;
					const FVec2 position = DirectionFronQuantanized(j, 32) * offset + Worms[i + 1].HeadPos();
					float Size = 5 + std::abs(value / 10.0f);

					auto color = FColor{ 255,255, 255 };

					float normal_attractiveness = value / 100.0f;
					float abs_attractiveness = (1.0f - std::abs(normal_attractiveness));

					if(normal_attractiveness < 0)
						color = FColor{ 255, (uint8_t)(255 * abs_attractiveness), (uint8_t)(255 * abs_attractiveness) };
					else if (normal_attractiveness > 0)
						color = FColor{ (uint8_t)(255 * abs_attractiveness), 255, (uint8_t)(255 * abs_attractiveness) };
					
					Dots(Video, Video.ViewPort(), ViewPortTransform, 1, &position, 0, &Size, 0, &color, 0);
				}
		}	
}

bool GameContext::RaycastWalls(const FVec2& senseDirection, const FVec2& rayStart, const FVec2& rayEndPoint, FSenseResult& out)
{
	auto OutsideDistance = std::min(
		std::min(WorldBounds.End.X - rayEndPoint.X, rayEndPoint.X - WorldBounds.Begin.X),
		std::min(WorldBounds.End.Y - rayEndPoint.Y, rayEndPoint.Y - WorldBounds.Begin.Y));

	if (OutsideDistance < 0.0f)
	{
		out = FSenseResult{
			ESenseHitType::Wall,
			rayStart,
			senseDirection,
			100.0f + OutsideDistance,
			0 };
		return true;
	}

	return false;
}

bool GameContext::RaycastWorms(const FVec2& senseDirection, const int wormIndex, const FVec2& rayStart, const FVec2& rayEndPoint, FSenseResult& out)
{
	for (size_t i = 0; i < Worms.size(); i++)
	{
		if(wormIndex == i)
		{
			if(GameRules & EGameRules_SelfCollisionCuts || GameRules & EGameRules_SelfCollisionKills)
			{
				for (int point_index = 0; point_index < (int)Worms[i].Points.size() - SELF_COLLISION_OFFSET; point_index++)
				{
					FVec2 point = Worms[i].Points[point_index];
					if (LineIntersectsCircle(rayStart, rayEndPoint, Worms[wormIndex].HeadSize() * .5f, point, Worms[i].HeadSize() * .5f))
					{
						out = FSenseResult{
						ESenseHitType::Worm,
						rayStart,
						senseDirection,
						GameRules & EGameRules_SelfCollisionKills ? (rayStart - point).Norm() : RAYCAST_DISTANCE,
						0 };
						return true;
					}
				}
				continue;
			}else
				continue;
		}

		if(LineIntersectsRectangle(rayStart, rayEndPoint, Worms[i].HeadSize(), Worms[i].Bounds))
		{
			for (FVec2 point : Worms[i].Points)
			{
				if(LineIntersectsCircle(rayStart, rayEndPoint, Worms[wormIndex].HeadSize() * .5f, point, Worms[i].HeadSize() * .5f))
				{
					out = FSenseResult{
					ESenseHitType::Worm,
					rayStart,
					senseDirection,
					(rayStart-point).Norm(),
					0 };
					return true;
				}				
			}			
		}
	}

	return false;
}

bool GameContext::RaycastFood(const FVec2& senseDirection, const FVec2& rayStart, const FVec2& rayEndPoint, float cast_width, FSenseResult& out)
{
	unsigned food_cell_indicies[9]{};

	unsigned food_cells_count = GetAdjacentFoodCells(rayStart, &food_cell_indicies[0]);
	for (unsigned j = 0; j < food_cells_count; j++)
	{
		auto& food_cell = *FoodGridPtrs[food_cell_indicies[j]];
		for (size_t food_index = 0; food_index < food_cell.size(); food_index++)
		{
			const auto& food = food_cell[food_index];
			if (LineIntersectsCircle(rayStart, rayEndPoint, cast_width, food.Position, FWormsVideoHelp::FoodRadius(3)))
			{
				out = FSenseResult{
					ESenseHitType::Food,
					rayStart,
					senseDirection,
					(rayStart - food.Position).Norm(),
					food.Value };

				return true;
			}
		}
	}

	return false;
}

bool GameContext::ProcessSenseDirection(const FVec2& senseDirection, const int wormIndex, FSenseResult& result)
{
	const auto& rayStart = Worms[wormIndex].HeadPos();
	const auto rayEndPoint = rayStart + senseDirection * RAYCAST_DISTANCE;
				
	if (RaycastWalls(senseDirection, rayStart,  rayEndPoint, result))
		return true;
						
	if (RaycastWorms(senseDirection, wormIndex, rayStart,  rayEndPoint, result))
		return true;

	if (RaycastFood(senseDirection, rayStart, rayEndPoint, Worms[wormIndex].HeadSize(), result))
		return true;

	return false;
}

void GameContext::UpdateAI(const FTime& Time)
{
	for (size_t i = 1; i < Worms.size(); i++)
	{
		std::unique_ptr<IWormAI>& ai = AIs[i - 1];
		auto& sensor = AISensors[i - 1];
		ai->Possess(Worms[i], sensor);
		auto senseDirections = sensor.TakeSenseDirections();

		for (FVec2& senseDirection : senseDirections)
		{
			FSenseResult result = {};

			if (ProcessSenseDirection(senseDirection, (int)i, result))
				sensor.AddSenseResult(result);
		}

		ai->Update(Time);
	}
}

void GameContext::ChangeState(GameState *state, const FTime& Time)
{
	delete _currentState;
	_currentState = state;
	_currentState->SetContext(this);
	_currentState->OnEnter(Time);
}

bool GameContext::ProcessDamagedWorms(const FTime& Time)
{
	for (int worm_index = (int)Worms.size() - 1; worm_index >= 0; --worm_index)
	{
		FWorm& worm = Worms[worm_index];

		if(worm.DeadSegments <= 0)
			continue;

		if(IsPlayerWorm(&worm))
			WormParts.emplace_back(WormPart{FWorm{worm.Points.front()}, PLAYER_COLORS });
		else
			WormParts.emplace_back(WormPart{ FWorm{worm.Points.front()}, OTHERS_COLORS });

		WormParts.back().Points().clear();

		for(auto it = worm.Points.begin(); it < std::next(worm.Points.begin(), worm.DeadSegments); it++)
		{
			auto copy = *it;
			WormParts.back().Points().emplace_back(copy);				
		}

		JukeBox.DamageTone(Time, worm.DeadSegments);

		if (worm.DeadSegments >= worm.HP())
		{
			if (IsPlayerWorm(&worm))
			{
				worm.Points.erase(worm.Points.begin(), std::next(worm.Points.begin(), worm.DeadSegments - 1));
				worm.DeadSegments = 0;
				return false;
			}

			AIs[worm_index - 1].release();
			AIs.erase(std::next(AIs.begin(), worm_index-1));
			Worms.erase(std::next(Worms.begin(), worm_index));
		}
		else
		{
			worm.Points.erase(worm.Points.begin(), std::next(worm.Points.begin(), worm.DeadSegments));
			worm.DeadSegments = 0;
		}
	}

	return true;
}

void GameContext::ProcessOverlaps(const FTime& Time)
{
	if(Time.DeltaTime == 0)
		return;

	unsigned food_cell_indicies[9]{};

	// food
	for (size_t worm_index = 0; worm_index < Worms.size(); worm_index++)
	{
		auto& worm = Worms[worm_index];
		unsigned food_cells_count = GetAdjacentFoodCells(worm.HeadPos(), &food_cell_indicies[0]);
		for (unsigned j = 0; j < food_cells_count; j++)
		{
			auto& food_cell = *FoodGridPtrs[food_cell_indicies[j]];
			for (size_t food_index = 0; food_index < food_cell.size(); food_index++)
			{
				const auto& food = food_cell[food_index];								

				const float distance = VecLength(food.Position - worm.HeadPos());
				if (distance < worm.HeadSize() * .5f + FWormsVideoHelp::FoodRadius(food.Value))
				{
					for (int k = 0; k < food.Value; k++)
						worm.Points.insert(worm.Points.begin(), worm.Points.front());					

					if (IsPlayerWorm(&worm))
						JukeBox.FoodConsumedTone(Time, food.Value);

					food_cell.erase(std::next(food_cell.begin(), food_index));
					break;
				}
			}
		}
	}

	//worm 2 worm
	for (size_t worm_index = 0; worm_index < Worms.size(); worm_index++)
	{
		auto& worm = Worms[worm_index];

		for (size_t another_worm_index = 0; another_worm_index < Worms.size(); another_worm_index++)
		{
			if (worm_index == another_worm_index)
				continue;

			FWorm& another_worm = Worms[another_worm_index];
			const float contact_distance = (worm.HeadSize() + another_worm.HeadSize()) * .5f;

			if (!Inside(worm.HeadPos(), another_worm.Bounds))
				continue;

			for (unsigned i = 0; i < another_worm.Points.size(); i++)
			{
				if ((worm.HeadPos() - another_worm.Points[i]).Norm() < contact_distance)
				{
					if (GameRules & EGameRules_CollidingIntoWormCutsIt)
						another_worm.DeadSegments = i + 1;

					if (GameRules & EGameRules_CollidingIntoWormsKillsYou)
						worm.DeadSegments = (int) worm.Points.size();
				}
			}
		}
	}

	//bounds
	for (size_t worm_index = 0; worm_index < Worms.size(); worm_index++)
		if (!Inside(Worms[worm_index].HeadPos(), WorldBounds))
			Worms[worm_index].DeadSegments = (int) Worms[worm_index].Points.size();

	if(GameRules & EGameRules_SelfCollisionCuts || GameRules & EGameRules_SelfCollisionKills)
		for (size_t worm_index = 0; worm_index < Worms.size(); worm_index++)
		{
			auto& worm = Worms[worm_index];
			for (int i = 0; i < (int)worm.Points.size() - SELF_COLLISION_OFFSET; i++)
			{
				if ((worm.HeadPos() - worm.Points[i]).Norm() < worm.HeadSize())
				{
					if (GameRules & EGameRules_SelfCollisionCuts)
						worm.DeadSegments = i + 1;

					if (GameRules & EGameRules_SelfCollisionKills)
						worm.DeadSegments = (int)worm.Points.size();
				}
			}
		}

}

void GameContext::ProcessDeadParts(const FTime& Time)
{
	if(Time.ElapsedTime < _nextPartsUpdate)
		return;

	_nextPartsUpdate = Time.ElapsedTime + DEAD_PARTS_UPDATE;

	for (size_t i = 0; i < WormParts.size(); i++)
	{
		auto& points = WormParts[i].Points();
		if (!points.empty())
		{
			FVec2 point = points.back();
			GetFoodCell(point).push_back(Food{ point, 1 });
			points.pop_back();
		}

		if (!points.empty())
		{
			FVec2 point = *points.begin();
			GetFoodCell(point).push_back(Food{ point, 1 });
			points.erase(points.begin());
		}
	}
}

void GameContext::LoopGameTheme(const FTime& Time)
{
	JukeBox.LoopBass(Time);
	JukeBox.LoopHighs(Time);
}

void GameContext::LoopGameOverTheme(const FTime& Time)
{
	JukeBox.LoopBass(Time);
}

void GameContext::PlayOpeningCountdown(const FTime& Time)
{
	Note highs[] = {
		{Tone_0, 8},
		{Tone_C4, 8},
		{Tone_0, 16},
		{Tone_E4, 8},
		{Tone_0, 16},
		{Tone_F4, 8},
	};

	JukeBox.PlayTune(highs, sizeof(highs) / sizeof(Note), 60, Time.ElapsedTime);
}

GameContext::~GameContext()
{
	for (size_t i = 0; i < GRID_COLUMNS * GRID_ROWS; i++)
		delete FoodGridPtrs[i];

	delete _currentState;
}

std::vector<Food>& GameContext::GetFoodCell(FVec2 position)
{
	position += FVec2(WORLD_WIDTH, WORLD_HEIGHT) * .5f;
	int col = static_cast<int>(position.Y / GRID_HEIGHT);
	int row =  + static_cast<int>(position.X / GRID_WIDTH);
	return *FoodGridPtrs[col * GRID_ROWS + row];
}

unsigned GameContext::GetAdjacentFoodCells(FVec2 position, unsigned cells[9])
{
	position += FVec2(WORLD_WIDTH, WORLD_HEIGHT) * .5f;
	const int col = static_cast<int>(position.Y / GRID_HEIGHT);
	const int row = static_cast<int>(position.X / GRID_WIDTH);
	int return_count = 0;

	for (int c_idnex = std::max(0, col-1); c_idnex <= std::min(col+1, GRID_COLUMNS-1); c_idnex++)
		for (int r_idnex = std::max(0, row-1); r_idnex <= std::min(row+1, GRID_ROWS-1); r_idnex++)
			cells[return_count++] = c_idnex * GRID_ROWS + r_idnex;

	return return_count;	
}


bool GameContext::Update(const FVideo& Video, const FAudio& Audio, const FInput& Input, const FTime& Time)
{
	AudioHelp.Silence(Audio);
	JukeBox.Update(AudioHelp, Time);

	const FVec2 ViewPortCenter{ Video.Width / 2.0f, Video.Height / 2.0f };
	const float ViewPortScale = Video.Width / 1920.0f;
	FVec2 offset = ViewPortCenter - PlayerWorm().HeadPos() * ViewPortScale;
	ViewPortTransform = FViewportTransform{offset, ViewPortScale};

	const bool keep_playing = _currentState->Update(Input, Time);

	AudioHelp.Mix(Audio);			
	Render(Video);

	return keep_playing;
}

GameContext* Game;

bool GameUpdate(const FVideo& Video, const FAudio& Audio, const FInput& Input, const FTime& Time)
{
	if (Game == nullptr)
	{
		Game = new GameContext{ Time };
		Game->DebugDisplay = EDebugDisplay_AI;
		Game->GameRules =
			EGameRules_CollidingIntoWormsKillsYou
			| EGameRules_CollidingIntoWormCutsIt
			//| EGameRules_SelfCollisionCuts
			//| EGameRules_SelfCollisionKills
			;
	}

	if (!Game->Update(Video, Audio, Input, Time))
	{
		delete Game;
		Game = nullptr;
	}

	if (Input.Escape)
		return false;

	return true;
}