#pragma once
#include "ecs.h"

// gets input and updates the transform postion based on the speed in the component
class PlayerUpdateSystem : public System
{
public:
	SYSTEM_CONSTRUCTOR(PlayerUpdateSystem);

	void Update() override;
};

class ColorCyclerSystem : public System
{
public:
	SYSTEM_CONSTRUCTOR(ColorCyclerSystem);

	void Update() override;
};

// spins a transform based on the speed in the component
class SpinnerSystem : public System
{
public:
	SYSTEM_CONSTRUCTOR(SpinnerSystem);

	void Update() override;
};

// draws all shapes that have a transform in the color they specify
class RenderSystem : public System
{
public:
	SYSTEM_CONSTRUCTOR(RenderSystem);

	void Update() override;
};

class Collision2dComponent;

// detects basic collisions on objects
class CollisionSystem : public System
{
public:
	SYSTEM_CONSTRUCTOR(CollisionSystem);

	void Update() override;

protected:
	bool Collision(Collision2dComponent* mover, Collision2dComponent* obstacle);
};