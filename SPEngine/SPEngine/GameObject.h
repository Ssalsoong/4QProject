#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <string>

class Component;
class Transform;
class GameObject
{
public:
	GameObject();

	template<typename T, typename... Args>
	std::weak_ptr<T> AddComponent(Args&&... args);
	template<typename T>
	std::weak_ptr<T> GetComponent();

	int id;
	std::wstring name = L"GameObject";
	std::wstring tag = L"Default";
	std::unordered_map<std::type_index, std::shared_ptr<Component>> components;
	std::weak_ptr<Transform> transform;

	virtual void Start();
	virtual void Update();
};

// 컴포넌트 추가
template<typename T, typename... Args>
std::weak_ptr<T> GameObject::AddComponent(Args&&... args)
{
	static_assert(std::is_base_of<Component, T>::value, "AddComponent<T> : T must derive from Component!!");

	auto comp = std::make_shared<T>(std::forward<Args>(args)...);
	comp->gameObject = this;
	components[typeid(T)] = comp;
	return comp;
}

// 컴포넌트 찾기
template<typename T>
std::weak_ptr<T> GameObject::GetComponent()
{
	static_assert(std::is_base_of<Component, T>::value, "GetComponent<T> : T must derive from Component!!");

	auto it = components.find(typeid(T));
	if (it == components.end())
		return nullptr;
	return std::static_pointer_cast<T>(it->second);
}