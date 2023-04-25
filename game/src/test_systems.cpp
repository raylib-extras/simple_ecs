#include "test_systems.h"
#include "test_components.h"
#include "ecs.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

void PlayerUpdateSystem::Update()
{
	DoForEachComponent<PlayerInputComponent>([this](PlayerInputComponent& component)
		{
			float deltaSpeed = component.LinearSpeed * GetFrameTime();

			TransformComponent* transform = ECSContainer.GetComponent<TransformComponent>(component.EntityId);
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

void RenderSystem::Update()
{
	DoForEachComponent<TransformComponent>([this](TransformComponent& component)
		{
			rlPushMatrix();
			rlTranslatef(component.Position.x, component.Position.y, 0);
			rlRotatef(component.Angle, 0, 0, 1);
			
			Color tint = WHITE;
			ColorComponent* color = ECSContainer.GetComponent<ColorComponent>(component.EntityId);
			if (color)
				tint = color->GetColor();

			CircleComponent* circle = ECSContainer.GetComponent<CircleComponent>(component.EntityId);
			if (circle)
				DrawCircleV(Vector2Zero(), circle->Radius, tint);

			RectangleComponent* rectangle = ECSContainer.GetComponent<RectangleComponent>(component.EntityId);
			if (rectangle)
				DrawRectangleRec(rectangle->Bounds, tint);

			rlPopMatrix();
		});
}

void SpinnerSystem::Update()
{
	DoForEachComponent<SpinnerComponent>([this](SpinnerComponent & component)
		{
			float deltaSpeed = component.RotationSpeed * GetFrameTime();

			TransformComponent* transform = ECSContainer.GetComponent<TransformComponent>(component.EntityId);
			if (!transform)
				return;

			transform->Angle += deltaSpeed;
		});
}

void ColorCyclerSystem::Update()
{
	DoForEachComponent<ColorComponent>([this](ColorComponent& component)
		{
			component.UpdateColor();
		});
}
