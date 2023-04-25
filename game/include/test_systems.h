#pragma once
#include "systems.h"

class PlayerUpdateSystem : public System
{
public:
	SYSTEM_CONSTRUCTOR(PlayerUpdateSystem);

	void Run() override;
};

class SpinnerSystem : public System
{
public:
	SYSTEM_CONSTRUCTOR(SpinnerSystem);

	void Run() override;
};

class RenderSystem : public System
{
public:
	SYSTEM_CONSTRUCTOR(RenderSystem);

	void Run() override;
};