## Raylib Simple ECS
Very simple ECS example

# Work In Progress
This is a work in progress example. It is functional but may not be fully documented/commented

## Building
Uses game-premake https://github.com/raylib-extras/game-premake

### Windows MinGW
Run the premake-mingw.bat and then run make in the folder

### Windows Visual Studio (not VSC)
Run premake-VisualStudio.bat and then open the fasteroids.sln that is generated

### Linux
CD into the directory, run ./premake5 gmake2 and then run make

#### MacOS
CD into the directory, run ./premake5.osx gmake2 and then run make


# ECS Basics
The goal of an ECS is to break gamplay objects into discrete components and have systems that itterate over those components quickly to do things.
An entity ID is used to link components that apply to the same game object together.
ECS frameworks are very common in the game industry because they make it easy to build up game objects via composition (a bundle of parts) rather than inheritance (a tree of dependent types).
They also keep things that are processed together near each other in memory to help prevent cache misses when processing items.

This example is a very basic ECS implementation and just meant as an introduction to the concepts.

## Entity
An enity is just an ID. It has no inherent data. It's just a way to define components that are attached to the same logical 'thing'. Every component has an Entity ID and components of different types may use the same entity ID, indicating that they are both part of the same entity. There is no 'master list' of entities, because entities themselves are never itterated, only components.

The entity ID is just a number.

## Components
A component is a thing that has data. Components have an entity ID to reference what entity is is attached too. Components contain data that is used by systems such as;
 * Transform
 * Color
 * Shape

Components are defined as class that is derived from Component and includes some common functions. Component Types have a unique ID so they can be referenced.

## Component Tables
All components of a single type are stored together in a component table. At it's core the table is just a big vector of components. This allows rapid itteration of components by systems, and can help minimize cache misses. many components will be loaded into cache because they are in the same memory block in the vector.

The component table is a generic interface to these lists of components and contains a map of Entity IDs to component index in the vector. This allows one a system to look up other compoonents for a specific entity ID. An example of this is when the render system has to look up the color component for an entity with a transform.

## System
A system is something that itterates one or more component types and does something with the entities that contain them. The RenderSystem is the most basic example of a system. It itterates all the entities with a transform, gets color and shape compoonents and then draws the results to the screen.

## ECS
This is simply a container for a set of component tables and registersed systems. It's all the data and logic that makes an ECS instance. It's a class because you may have multiple ECS systems for different uses (multiple rooms on a server for instance).


# Code Structure

## ECS library
The ECS library contains the base code for the system. It has no actuall components or systems, just the base classes and code used.

To use the ECS you define a new variable fo the type ECS;

```
ECS myECS;
```

## Defining a component
You define a component by creating a class derived from Component. You must call the DEFINE_COMPONENT macro in the public section of your class. This macro is what addes in the code needed to define the unqique component ID for your component type. The passed in argument is used to create this code.

```
class YourComponent : public Component
{
public:
	DEFINE_COMPONENT(YourComponent);

	int SomeData = 0;
	float SomeOtherData = 1.234f;
};
```
You can then define any other data you want. You can also define methods on the component if you wish. Systems will have full acceess to the component class when processing it.

## Registering a component
Components are registerd with the ECS set with a call to RegisterComponent. This will create the component table for this type.

```
myECS.RegisterComponent<YourComponent>();
```

## Creating entities
Entities are just an ID, so they are created with a call to GetNewEntity() on the ECS.

```
uint64_t newEntity = myECS..GetNewEntity();
```

## Adding components to entities
You can add a component to an entity by simply asking the ECS for the component on that entity, if the component does not exist for that entity it'll be created. GetComponent will return a pointer to the component for the entity. 

```
YourComponent* componentPtr = myECS.GetComponent<YourComponent>(newEntity);
```

## Setting data in components
Once you have a component pointer, just manipulate the data in it as normal
```
componentPtr->SomeData = 1000;
```

## Defining Systems
Systems are defined similar to components by deriving from the System class.
```
class MySystem : public System
{
public:
	SYSTEM_CONSTRUCTOR(MySystem);

	void Update() override;
};
```
Like components, you must call the macro SYSTEM_CONSTRUCTOR in the public part of your class. This defines the correct construtor needed for systems to work.

### The update method in systems
The update method is called when your system is run. Within it you can do logic on all compnents of one or multiple types using the DoForEachComponent method with a callback or lambda. You can also manually itterate components yourelf by getting the component table for a component type from the ECS. Every System has a reference to the ECS it is registered with as the ECSContainer member.

```
void MySystem::Update()
{
	DoForEachComponent<YourComponent>([this](YourComponent& component)
		{
			if (component->SomeData > 10)
				component->SomeOtherData = 5;
		});
}
```

## Registering Systems
Systems are registered the same way as components, using the ECS object.
```
myECS.RegisterSystem<MySystem>();
```

## Updating the ECS
calling ECS::Update(); will update all systems in the order they are registered.
```
myECS.Update();
```

# More reading
* https://en.wikipedia.org/wiki/Entity_component_system
* https://www.simplilearn.com/entity-component-system-introductory-guide-article
* Entity Component System Overview in 7 Minutes https://www.youtube.com/watch?v=2rW7ALyHaas
* Is There More to Game Architecture than ECS? https://www.youtube.com/watch?v=JxI3Eu5DPwE

