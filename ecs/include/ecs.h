#pragma once
#include "components.h"
#include <memory>

class System;


// a container for a set of entities, and a set of registered systems
class ECS
{
public:
	std::vector<std::unique_ptr<System>> Systems;
	std::unordered_map <size_t, ComponentTableBase*> Tables;

	void Update();

	template<class T>
	inline T* RegisterSystem()
	{
		Systems.emplace_back(std::move(std::make_unique<T>(*this)));
		return reinterpret_cast<T*>(Systems.back().get());
	}

	template <class T>
	ComponentTable<T>* RegisterComponent()
	{
		ComponentTableBase* tablePtr = new ComponentTable<T>();
		Tables[T::GetComponentId()] = tablePtr;

		return GetComponentTable<T>();
	}

	template <class T>
	ComponentTable<T>* GetComponentTable()
	{
		return reinterpret_cast<ComponentTable<T>*>(Tables[T::GetComponentId()]);
	}

	template <class T>
	T* GetComponent(uint64_t id)
	{
		auto table = Tables.find(T::GetComponentId());
		if (table == Tables.end())
			return nullptr;

		return reinterpret_cast<T*>(table->second->Get(id));
	}

	template <class T>
	T* TryGetComponent(uint64_t id)
	{
		auto table = Tables.find(T::GetComponentId());
		if (table == Tables.end())
			return nullptr;

		return reinterpret_cast<T*>(table->second->TryGet(id));
	}

	uint64_t GetNewEntity();

	void RemoveEntity(uint64_t id);

	virtual ~ECS()
	{
		for (auto table : Tables)
			delete(table.second);
	}

protected:
	std::stack<uint64_t> DeadIds;
	uint64_t NextId = 0;
};