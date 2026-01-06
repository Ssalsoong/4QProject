#pragma once
#include "Object.h"
#include "rttr/type"

namespace MMMEngine
{
	class GameObject;
	class Component : public Object
	{
	private:
		friend class App;
		friend class ObjectManager;
		friend class GameObject;
		RTTR_ENABLE(MMMEngine::Object)
		RTTR_REGISTRATION_FRIEND

		ObjectPtr<GameObject> m_owner;
	protected:
		Component() = default;
	public:
		virtual ~Component() = default;
	};
}