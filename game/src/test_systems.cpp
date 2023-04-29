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

			TransformComponent* transform = ECSContainer.TryGetComponent<TransformComponent>(component.EntityId);
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


void PushTransformComponent(const TransformComponent& transform, ECS& ecs)
{
	if (transform.Parent != uint64_t(-1))
	{
		TransformComponent* parentTransform = ecs.TryGetComponent<TransformComponent>(transform.Parent);
		if (parentTransform)
			PushTransformComponent(*parentTransform, ecs);
	}

	rlPushMatrix();
	rlTranslatef(transform.Position.x, transform.Position.y, 0);
	rlRotatef(transform.Angle, 0, 0, 1);
}

void PopTransformComponent(const TransformComponent& transform, ECS& ecs)
{
	rlPopMatrix();

	if (transform.Parent != uint64_t(-1))
	{
		TransformComponent* parentTransform = ecs.TryGetComponent<TransformComponent>(transform.Parent);
		if (parentTransform)
			PopTransformComponent(*parentTransform, ecs);
	}
}

void RenderSystem::Update()
{
	DoForEachComponent<TransformComponent>([this](TransformComponent& component)
		{
			PushTransformComponent(component, ECSContainer);
			
			Color tint = WHITE;
			ColorComponent* color = ECSContainer.TryGetComponent<ColorComponent>(component.EntityId);
			if (color)
				tint = color->GetColor();

			CircleComponent* circle = ECSContainer.TryGetComponent<CircleComponent>(component.EntityId);
			if (circle)
				DrawCircleV(Vector2Zero(), circle->Radius, tint);

			RectangleComponent* rectangle = ECSContainer.TryGetComponent<RectangleComponent>(component.EntityId);
			if (rectangle)
				DrawRectangleRec(rectangle->Bounds, tint);

			PopTransformComponent(component, ECSContainer);
		});
}

void SpinnerSystem::Update()
{
	DoForEachComponent<SpinnerComponent>([this](SpinnerComponent & component)
		{
			float deltaSpeed = component.RotationSpeed * GetFrameTime();

			TransformComponent* transform = ECSContainer.TryGetComponent<TransformComponent>(component.EntityId);
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

void CollisionSystem::Update()
{
	std::vector<Collision2dComponent*> staticEntities;
	std::vector<Collision2dComponent*> dynamicEntities;

	DoForEachComponent<Collision2dComponent>([this, &staticEntities, &dynamicEntities](Collision2dComponent& component)
		{
			if (component.IsStatic)
				staticEntities.push_back(&component);
			else
				dynamicEntities.push_back(&component);
		});

	for (Collision2dComponent* mover : dynamicEntities)
	{
		// check against all the static entities
		for (Collision2dComponent* obstacle : staticEntities)
		{
			if (Collision(mover, obstacle))
			{
				if (mover->OnCollide)
					mover->OnCollide(mover->EntityId, obstacle->EntityId, ECSContainer);

				if (obstacle->OnCollide)
					obstacle->OnCollide(obstacle->EntityId, mover->EntityId, ECSContainer);
			}
		}

		// check against the other non dynamic entities
		for (Collision2dComponent* obstacle : dynamicEntities)
		{
			if (obstacle != mover && Collision(mover, obstacle))
			{
				if (mover->OnCollide)
					mover->OnCollide(mover->EntityId, obstacle->EntityId, ECSContainer);

				if (obstacle->OnCollide)
					obstacle->OnCollide(obstacle->EntityId, mover->EntityId, ECSContainer);
			}
		}
	}
}

bool CollisionSystem::Collision(Collision2dComponent* mover, Collision2dComponent* obstacle)
{
	if (!mover || !obstacle)
		return false;

	TransformComponent* moverTransform = ECSContainer.TryGetComponent<TransformComponent>(mover->EntityId);
	TransformComponent* obstacleTransform = ECSContainer.TryGetComponent<TransformComponent>(obstacle->EntityId);
	if (!moverTransform || !obstacleTransform)
		return false;


	CircleComponent* moverCircle = ECSContainer.TryGetComponent<CircleComponent>(mover->EntityId);
	RectangleComponent* moverRectangle = ECSContainer.TryGetComponent<RectangleComponent>(mover->EntityId);
	if (!moverCircle && !moverRectangle)
		return false;

	CircleComponent* obstacleCircle = ECSContainer.TryGetComponent<CircleComponent>(obstacle->EntityId);
	RectangleComponent* obstacleRectangle = ECSContainer.TryGetComponent<RectangleComponent>(obstacle->EntityId);

	if (!obstacleCircle && !obstacleRectangle)
		return false;

	if (obstacleCircle && moverCircle) // circle circle, easy
		return CheckCollisionCircles(moverTransform->Position, moverCircle->Radius, obstacleTransform->Position, obstacleCircle->Radius);

	// utility to help us fix up rectangles in the transform space
	auto translateRect = [](const Rectangle& r, const Vector2& pos)
	{
		return Rectangle{ pos.x + r.x, pos.y + r.y, r.width,r.height };
	};

	if (obstacleRectangle && moverRectangle) // rect rect, easy
		return CheckCollisionRecs(translateRect(moverRectangle->Bounds, moverTransform->Position), translateRect(obstacleRectangle->Bounds, obstacleTransform->Position));

	if (obstacleRectangle && moverCircle) // rect circle
		return CheckCollisionCircleRec(moverTransform->Position, moverCircle->Radius, translateRect(obstacleRectangle->Bounds, obstacleTransform->Position));

	if (obstacleCircle && moverRectangle) // rect circle
		return CheckCollisionCircleRec(obstacleTransform->Position, obstacleCircle->Radius, translateRect(moverRectangle->Bounds, moverTransform->Position));

	return false;
}
