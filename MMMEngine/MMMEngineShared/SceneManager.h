#pragma once
#include "Singleton.hpp"

namespace MMMEngine
{
	class SceneManager : public Utility::Singleton<SceneManager>
	{
		void StartUp();
		void ShutDown();
		void Update();
	};
}