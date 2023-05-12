#pragma once

#include <memory>
#include <functional>
#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <stack>

class System;

// base class for a component
class Component
{
public:
    uint64_t EntityId = uint64_t(-1);									// entity that is attached to this component
    inline virtual size_t GetTypeId() const { return size_t(-1); }		// a way to get the type ID of this component when all you have is the base class

    virtual ~Component() = default;
};

// macro to define common code that all components need
// computes a runtime unique ID for each component using the name
// NOTE that this ID is not deterministic, a real ECS would hash the name and use a better ID
#define DEFINE_COMPONENT(T) \
static const char* GetComponentName() { return #T;}  \
static size_t GetComponentId() { return reinterpret_cast<size_t>(#T); } \
inline size_t GetTypeId() const override { return GetComponentId(); }

// base class for a generic table of components that the Entity Set can hold and not care about the type
class ComponentTableBase
{
public:
    virtual Component* Add(uint64_t id) = 0;
    virtual void Remove(uint64_t id) = 0;
    virtual bool ContainsEntity(uint64_t id) = 0;
    virtual Component* Get(uint64_t id) = 0;
    virtual Component* TryGet(uint64_t id) = 0;

    virtual ~ComponentTableBase() = default;
};

// specific version of table class that stores the entities in a vector.
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

    // gets or adds a component for this entity ID
    Component* Get(uint64_t id) override
    {
        std::unordered_map<uint64_t, size_t>::iterator entityItr = EntityIds.find(id);
        if (entityItr == EntityIds.end())
            return Add(id);

        return &Components[entityItr->second];
    }

    // gets or adds a component for this entity ID
    Component* TryGet(uint64_t id) override
    {
        std::unordered_map<uint64_t, size_t>::iterator entityItr = EntityIds.find(id);
        if (entityItr == EntityIds.end())
            return nullptr;

        return &Components[entityItr->second];
    }

protected:
    // when we remove a component, all the IDs shift, so rebuild the entity ID map
    void BuildIdCache()
    {
        EntityIds.clear();
        for (size_t i = 0; i < Components.size(); i++)
            EntityIds.insert_or_assign(Components[i].EntityId, i);
    }

    // a map of entity IDs to component index to make lookups fast
    std::unordered_map<uint64_t, size_t> EntityIds;
};


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