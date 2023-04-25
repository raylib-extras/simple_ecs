#pragma once

#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <stack>
#include <memory>

class Component
{
public:
	uint64_t EntityId = uint64_t(-1);
	inline virtual size_t GetTypeId() const { return size_t(-1); }

	virtual ~Component() = default;
};

#define DEFINE_COMPONENT(T) \
static const char* GetComponentName() { return #T;}  \
static size_t GetComponentId() { return reinterpret_cast<size_t>(#T); } \
inline size_t GetTypeId() const override { return GetComponentId(); }


class ComponentTableBase
{
public:
	virtual Component* Add(uint64_t id) = 0;
	virtual void Remove(uint64_t id) = 0;
	virtual bool ContainsEntity(uint64_t id) = 0;
	virtual Component* Get(uint64_t id) = 0;

	virtual ~ComponentTableBase() = default;
};

template <class T>
class ComponentTable : public ComponentTableBase
{
public:
	std::vector<T> Components;

	inline Component* Add(uint64_t id) override
	{
		Components.emplace_back(T());
		T* component = &Components.back();
		component->EntityId = id;
		EntityIds.insert_or_assign(id, Components.size() - 1);

		return component;
	}

	void Remove(uint64_t id) override
	{
		auto slot = EntityIds.find(id);
		if (slot == EntityIds.end())
			return;

		Components.erase(Components.begin() + slot->second);
		BuildIdCache();
	}

	bool ContainsEntity(uint64_t id) override
	{
		return EntityIds.find(id) != EntityIds.end();
	}

	Component* Get(uint64_t id) override
	{
		std::unordered_map<uint64_t, size_t>::iterator entityItr = EntityIds.find(id);
		if (entityItr == EntityIds.end())
			return Add(id);

		return &Components[entityItr->second];
	}

protected:
	void BuildIdCache()
	{
		EntityIds.clear();
		for (size_t i = 0; i < Components.size(); i++)
			EntityIds.insert_or_assign(Components[i].EntityId, i);
	}

	std::unordered_map<uint64_t, size_t> EntityIds;
};

class EntitySet
{
public:
	template <class T>
	ComponentTable<T>* RegisterComponent()
	{
		ComponentTableBase* tablePtr = new ComponentTable<T>();
		Tables[T::GetComponentId()] = tablePtr;

		return GetComponents<T>();
	}

	template <class T>
	ComponentTable<T>* GetComponents()
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

	uint64_t GetNewEntity();

	void RemoveEntity(uint64_t id);

	virtual ~EntitySet()
	{
		for (auto table : Tables)
			delete(table.second);
	}

protected:
	std::unordered_map <size_t, ComponentTableBase*> Tables;

	std::stack<uint64_t> DeadIds;
	uint64_t NextId = 0;
};