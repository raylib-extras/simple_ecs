/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

For a C++ project simply rename the file to .cpp and run premake 

*/

#include "raylib.h"

#include "test_components.h"
#include "test_systems.h"
#include "entity_component_system.h"

int main ()
{
	// set up the window
	InitWindow(1280, 800, "Hello Raylib");
	SetTargetFPS(144);

	ECS ecs;

	// set up components
	ecs.RegisterComponent<TransformComponent>();
	ecs.RegisterComponent<ColorComponent>();
	ecs.RegisterComponent<CircleComponent>();
	ecs.RegisterComponent<RectangleComponent>();
	ecs.RegisterComponent<PlayerInputComponent>();
	ecs.RegisterComponent<SpinnerComponent>();

	// set up systems (updated in order)
	ecs.RegisterSystem<SpinnerSystem>();
	ecs.RegisterSystem<PlayerUpdateSystem>();
	ecs.RegisterSystem<RenderSystem>();

	// set up entities
	uint64_t blockId = ecs.Entities.GetNewEntity();
	ecs.Entities.GetComponent<TransformComponent>(blockId)->Position = Vector2{ 400,300 };
	ColorComponent* color = ecs.Entities.GetComponent<ColorComponent>(blockId);
	color->Tint = RED;
	color->TintB = PURPLE;
	color->TintSpeed = 1;

	ecs.Entities.GetComponent<RectangleComponent>(blockId)->Bounds = Rectangle{ -50,-50,100,100 };
	ecs.Entities.GetComponent<SpinnerComponent>(blockId)->RotationSpeed = 90;
	
	uint64_t playerID = ecs.Entities.GetNewEntity();
	ecs.Entities.GetComponent<TransformComponent>(playerID)->Position = Vector2{ 50,50 };
	ecs.Entities.GetComponent<ColorComponent>(playerID)->Tint = BLUE;
	ecs.Entities.GetComponent<CircleComponent>(playerID)->Radius = 25;
	ecs.Entities.GetComponent<PlayerInputComponent>(playerID)->LinearSpeed = 200;

	// game loop
	while (!WindowShouldClose())
	{
		// drawing
		BeginDrawing();
		ClearBackground(BLACK);

		ecs.Update();

		DrawFPS(5, 5);
		
		EndDrawing();
	}

	// cleanup
	CloseWindow();
	return 0;
}