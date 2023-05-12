
#pragma once

#include "ecs.h"

#include "raylib.h"
#include "raymath.h"

#include <functional>
#include <stdint.h>

class TransformComponent : public Component
{
public:
	DEFINE_COMPONENT(TransformComponent);

	Vector2 Position = { 0,0 };
	float Angle = 0;

	uint64_t Parent = uint64_t(-1);
};

class ColorComponent : public Component
{
public: 
	DEFINE_COMPONENT(ColorComponent);

	Color Tint = WHITE;
	Color TintB = PURPLE;
	float TintSpeed = 0;

	inline void UpdateColor()
	{
		TintParam += GetFrameTime() / TintSpeed;
	}

	inline Color GetColor()
	{
		if (TintSpeed == 0)
			return Tint;

		if (TintParam >= 1)
		{
			TintParam = 1;
			TintSpeed *= -1;
			return TintB;
		}

		if (TintParam <= 0)
		{
			TintParam = 0;
			TintSpeed *= -1;
			return Tint;
		}

		return Color{ 
			(unsigned char)Lerp(Tint.r,TintB.r, TintParam),
			(unsigned char)Lerp(Tint.g,TintB.g, TintParam),
			(unsigned char)Lerp(Tint.b,TintB.b, TintParam) ,
			(unsigned char)Lerp(Tint.a,TintB.a, TintParam) };
	}
protected:
	float TintParam = 0;
};

class CircleComponent : public Component
{
public:
	DEFINE_COMPONENT(CircleComponent);

	float Radius = 0;
};

class RectangleComponent : public Component
{
public:
	DEFINE_COMPONENT(RectangleComponent);

	Rectangle Bounds = { 0,0,0,0 };
};

class PlayerInputComponent : public Component
{
public:
	DEFINE_COMPONENT(PlayerInputComponent);

	float LinearSpeed = 100;
	float RotationSpeed = 180;
};

class SpinnerComponent : public Component
{
public:
	DEFINE_COMPONENT(SpinnerComponent);

	float RotationSpeed = 180;
};

class ECS;

class Collision2dComponent : public Component
{
public:
	DEFINE_COMPONENT(Collision2dComponent);

	// does this move or not? static items are not checked against each other
	bool IsStatic = false;

	// called when something collides with this
	//				   thisID    otherID   ECS ref
	std::function<void(uint64_t, uint64_t, ECS&)> OnCollide;
};