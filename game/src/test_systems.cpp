#include "test_systems.h"
#include "test_components.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

void PlayerUpdateSystem::Run()
{
	DoForEachComponent<PlayerInputComponent>([this](PlayerInputComponent& component)
		{
			float deltaSpeed = component.LinearSpeed * GetFrameTime();

			TransformComponent* transform = EntSet.GetComponent<TransformComponent>(component.EntityId);
			if (!transform)
				return;

			Vector2 move = { 0,0 };

			if (IsKeyDown(KEY_W))
				move.y -= deltaSpeed;

			if (IsKeyDown(KEY_S))
				move.y += deltaSpeed;

			if (IsKeyDown(KEY_A))
				move.x -= deltaSpeed;

			if (IsKeyDown(KEY_D))
				move.x += deltaSpeed;

			transform->Position = Vector2Add(transform->Position, move);
		});
}

void RenderSystem::Run()
{
	DoForEachComponent<TransformComponent>([this](TransformComponent& component)
		{
			rlPushMatrix();
			rlTranslatef(component.Position.x, component.Position.y, 0);
			rlRotatef(component.Angle, 0, 0, 1);
			
			Color tint = WHITE;
			ColorComponent* color = EntSet.GetComponent<ColorComponent>(component.EntityId);
			if (color)
				tint = color->GetColor();

			CircleComponent* circle = EntSet.GetComponent<CircleComponent>(component.EntityId);
			if (circle)
				DrawCircleV(Vector2Zero(), circle->Radius, tint);

			RectangleComponent* rectangle = EntSet.GetComponent<RectangleComponent>(component.EntityId);
			if (rectangle)
				DrawRectangleRec(rectangle->Bounds, tint);

			rlPopMatrix();
		});
}

void SpinnerSystem::Run()
{
	DoForEachComponent<SpinnerComponent>([this](SpinnerComponent & component)
		{
			float deltaSpeed = component.RotationSpeed * GetFrameTime();

			TransformComponent* transform = EntSet.GetComponent<TransformComponent>(component.EntityId);
			if (!transform)
				return;

			transform->Angle += deltaSpeed;
		});
}
