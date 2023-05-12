#include "ecs.h"

void ECS::Update()
{
	for (auto& system : Systems)
		system->Update();
}

// reuses an old ID or computes a new one
uint64_t ECS::GetNewEntity()
{
	uint64_t id = NextId;
	if (!DeadIds.empty())
	{
		id = DeadIds.top();
		DeadIds.pop();
	}
	else
	{
		NextId++;
	}

	return id;
}

// remove all components that use this ID, and mark it for reuse
void ECS::RemoveEntity(uint64_t id)
{
	DeadIds.push(id);

	for (auto& table : Tables)
		table.second->Remove(id);
}
