#include "components.h"
#include "systems.h"

#include <memory>

class ECS
{
public:
	EntitySet Entities;
	std::vector<std::unique_ptr<System>> Systems;

	void Update();

	template<class T>
	inline T* RegisterSystem()
	{
		Systems.emplace_back(std::move(std::make_unique<T>(Entities)));
		return reinterpret_cast<T*>(Systems.back().get());
	}

	template <class T>
	ComponentTable<T>* RegisterComponent()
	{
		return Entities.RegisterComponent<T>();
	}
};