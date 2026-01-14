#pragma once
#include "Export.h"
#include "Behaviour.h"

namespace MMMEngine
{
	class ScriptBehaviour : public Behaviour
	{
	public:
		ScriptBehaviour() = default;
		virtual ~ScriptBehaviour() = default;
	};
}