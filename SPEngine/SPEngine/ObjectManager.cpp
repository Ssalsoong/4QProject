#include "ObjectManager.h"
#include "GameObject.h"

std::weak_ptr<GameObject> ObjectManager::CreateObject()
{
	auto obj = std::make_shared<GameObject>();
	obj->id = nextId++;

	objects.push_back(obj);
	objectMap[obj->id] = obj;

	return obj;
}

bool ObjectManager::RemoveObject(std::shared_ptr<GameObject> _object)
{
	int id = _object->id;

	auto mapIt = objectMap.find(id);
	if (mapIt == objectMap.end())
		return false;

	removePendingObject.push(id);
	return true;
}

bool ObjectManager::RemoveObject(int id)
{
	auto mapIt = objectMap.find(id);
	if (mapIt == objectMap.end())
		return false;

	removePendingObject.push(id);
	return true;
}

void ObjectManager::Update()
{
	while (!removePendingObject.empty()) {
		int id = removePendingObject.top();
		removePendingObject.pop();

		auto mapIt = objectMap.find(id);
		if (mapIt != objectMap.end()) {
			auto object = mapIt->second;

			auto vecIt = std::find(objects.begin(), objects.end(), object);
			if (vecIt != objects.end()) {
				objects.erase(vecIt);
			}
			
			objectMap.erase(mapIt);
		}
	}

	// 오브젝트 업데이트
	for (const auto& obj : objects) {
		obj->Update();
	}
}
