#pragma once

#include "components.h"
#include <functional>
#include "ecs.h"

class System
{
public:
	System(ECS& ecsContainer)
		:ECSContainer(ecsContainer)
	{
	}

	virtual void Update() = 0;

protected:
	ECS& ECSContainer;

	template<class T>
	inline void DoForEachComponent(std::function<void(T&)> callback)
	{
		if (!callback)
			return;

		for (T& comp : ECSContainer.GetComponentTable<T>()->Components)
			callback(comp);
	}
};

#define SYSTEM_CONSTRUCTOR(T) T(ECS& ecsContainer) : System(ecsContainer){}