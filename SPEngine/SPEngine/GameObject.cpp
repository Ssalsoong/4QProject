#include "GameObject.h"
#include "Transform.h"
#include "Component.h"

GameObject::GameObject()
{
	transform = AddComponent<Transform>();
}

void GameObject::Start()
{
	for (const auto& comp : components)
		comp.second->Start();
}

void GameObject::Update()
{
	for (const auto& comp : components)
		comp.second->Update();
}
