/*
Raylib ECS Example
*/

#include "raylib.h"

#include "test_components.h"
#include "test_systems.h"
#include "ecs.h"

int main ()
{
	// set up the window
	InitWindow(1280, 800, "ECS Example");
	SetTargetFPS(144);

	// our ECS container
	ECS ecs;

	// set up components
	ecs.RegisterComponent<TransformComponent>();
	ecs.RegisterComponent<ColorComponent>();
	ecs.RegisterComponent<CircleComponent>();
	ecs.RegisterComponent<RectangleComponent>();
	ecs.RegisterComponent<PlayerInputComponent>();
	ecs.RegisterComponent<SpinnerComponent>();
	ecs.RegisterComponent<Collision2dComponent>();

	// set up systems (updated in order)
	ecs.RegisterSystem<SpinnerSystem>();
	ecs.RegisterSystem<ColorCyclerSystem>();
	ecs.RegisterSystem<PlayerUpdateSystem>();
	ecs.RegisterSystem<CollisionSystem>();
	ecs.RegisterSystem<RenderSystem>();

	// set up entities
	// static entity
	uint64_t blockId = ecs.GetNewEntity();
	ecs.GetComponent<TransformComponent>(blockId)->Position = Vector2{ 400,300 };
	ColorComponent* color = ecs.GetComponent<ColorComponent>(blockId);
	color->Tint = RED;
	color->TintB = ColorAlpha(PURPLE, 0.25f);
	color->TintSpeed = 1;

	ecs.GetComponent<RectangleComponent>(blockId)->Bounds = Rectangle{ -50,-50,100,100 };
	ecs.GetComponent<SpinnerComponent>(blockId)->RotationSpeed = 90;

	uint64_t obstacleId = ecs.GetNewEntity();
	ecs.GetComponent<TransformComponent>(obstacleId)->Position = Vector2{ 300, 400 };
	ecs.GetComponent<ColorComponent>(obstacleId)->Tint = DARKGREEN;
	ecs.GetComponent<RectangleComponent>(obstacleId)->Bounds = Rectangle{ -40,-40, 80, 80 };
	ecs.GetComponent<Collision2dComponent>(obstacleId)->IsStatic = true;

	
	// player entity
	uint64_t playerID = ecs.GetNewEntity();
	ecs.GetComponent<TransformComponent>(playerID)->Position = Vector2{ 50,50 };
	ecs.GetComponent<ColorComponent>(playerID)->Tint = BLUE;
	ecs.GetComponent<CircleComponent>(playerID)->Radius = 25;
	ecs.GetComponent<PlayerInputComponent>(playerID)->LinearSpeed = 200;
	Collision2dComponent* component = ecs.GetComponent<Collision2dComponent>(playerID);
	component->OnCollide = [](uint64_t entityId, uint64_t obstacleEntityId, ECS& ecsSystem)
	{
		ColorComponent* color = ecsSystem.GetComponent<ColorComponent>(entityId);
		if (color->Tint.r > BLUE.r)
			color->Tint = BLUE;
		else
			color->Tint = RED;
	};

	// game loop
	while (!WindowShouldClose())
	{
		// game loop
		BeginDrawing();
		ClearBackground(BLACK);

		// run all systems
		ecs.Update();

		DrawFPS(5, 5);
		
		EndDrawing();
	}

	// cleanup
	CloseWindow();
	return 0;
}