#include <iostream>

#define NOMINMAX
#include "App.h"
#include "GUID.h"
#include "GameObject.h"

using namespace MMMEngine;

#include <rttr/registration>
static void f() { std::cout << "Hello World" << std::endl; }
using namespace rttr;
RTTR_REGISTRATION
{
	using namespace rttr;
	registration::method("f", &f);
}

void Render() { std::cout << "drawcall" << std::endl; }

class Test
{
public:
	void Foo() { std::cout << "Foo called" << std::endl; }
};

class RollObject;
class FooObject : public Object
{
public:
	ObjectPtr<RollObject> pRollObj;
	void Foo()
	{
		auto inst = CreatePtr<GameObject>();
		if (inst)
		{
			std::cout << GetName() << std::endl;
			std::cout << inst->GetName() << std::endl;
		}
	}
};

class RollObject : public Object
{
public:
	void A()
	{
		auto FooPtr = FindObjectByType<FooObject>();
		FooPtr->pRollObj = SelfPtr(this);
		FooPtr->pRollObj->SetName("watashiga tenni tatsu");
	}
};

int main()
{
	App app;

	Test testObj;

	Action<> act;
	act.AddListener<&Render>();

	act.RemoveListener<&Render>();

	act.AddListener<Test, &Test::Foo>(&testObj);

	app.onRender.AddListener<&Render>();

	act.Invoke();

	app.Run();

	MMMEngine::GUID id = MMMEngine::GUID::NewGuid();
	
	type::invoke("f", {});

	std::cout << "Generated GUID: " << id.ToString() << std::endl;
	auto FooObj = Object::CreatePtr<FooObject>();
	FooObj->Foo();

	type class_type = type::get_by_name("ObjectPtr<GameObject>");
	if (class_type)
	{
		auto obj = class_type.create().get_value<ObjectPtr<GameObject>>();
		std::cout << obj->GetName() << std::endl;

		std::cout << (*obj).GetGUID() << std::endl;
	
		Object::Destroy(obj);

		if (obj)
			std::cout << "Alived!" << std::endl;
		else
			std::cout << "Destroyed!" << std::endl;
	}

	

	auto a = Object::CreatePtr<RollObject>();
	if(a) // È¤Àº a.IsValid()
		a->A();

	auto founds = Object::FindObjectsByType<Object>();

	for (auto& f : founds)
	{
		std::cout << f->GetName() << std::endl;
	}

	return 0;
}