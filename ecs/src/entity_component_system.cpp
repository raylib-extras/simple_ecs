#include "entity_component_system.h"

void ECS::Update()
{
	for (auto& system : Systems)
		system->Run();
}
