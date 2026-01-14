#pragma once
#include <vector>
#include <unordered_map>
#include <stack>
#include "Singleton.h"

class GameObject;
class ObjectManager : public SingleTon<ObjectManager>
{
	friend class SingleTon<ObjectManager>;
private:
	std::vector<std::shared_ptr<GameObject>> objects;
	std::stack<int> removePendingObject;
	std::unordered_map<int, std::shared_ptr<GameObject>> objectMap;
	int nextId = 0;

public:
	std::weak_ptr<GameObject> CreateObject();
	bool RemoveObject(std::shared_ptr<GameObject> _object);
	bool RemoveObject(int id);

	void Update();
};

