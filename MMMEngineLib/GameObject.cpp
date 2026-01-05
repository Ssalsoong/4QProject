#include "GameObject.h"
#include "rttr/registration"
#include "rttr/detail/policies/ctor_policies.h"


RTTR_REGISTRATION
{
	using namespace rttr;
	using namespace MMMEngine;

	registration::class_<GameObject>("GameObject");

	registration::class_<ObjectPtr<GameObject>>("ObjectPtr<GameObject>")
        .constructor<>(
			[]() {
				return Object::CreatePtr<GameObject>();
			}, registration::protected_access
        );
}
