#pragma once
#include "framework.h"

class GameObject;
class Component
{
public:
	// 부모객체 표시
	GameObject* gameObject = nullptr;

	virtual void Start() = 0;
	virtual void Update() = 0;
};

