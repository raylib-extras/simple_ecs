#pragma once

#include "components.h"
#include <functional>

class System
{
public:
	System(EntitySet& entSet)
		:EntSet(entSet)
	{
	}

	virtual void Run() = 0;

protected:
	EntitySet& EntSet;

	template<class T>
	inline void DoForEachComponent(std::function<void(T&)> callback)
	{
		if (!callback)
			return;

		for (T& comp : EntSet.GetComponents<T>()->Components)
			callback(comp);
	}
};

#define SYSTEM_CONSTRUCTOR(T) T(EntitySet& entSet) : System(entSet){}