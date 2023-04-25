#include "components.h"

uint64_t EntitySet::GetNewEntity()
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

void EntitySet::RemoveEntity(uint64_t id)
{
	DeadIds.push(id);

	for (auto& table : Tables)
		table.second->Remove(id);
}
