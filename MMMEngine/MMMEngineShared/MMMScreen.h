#pragma once
#include "GlobalRegistry.h"
//#include "RenderManager.h"

#include "SimpleMath.h"
#include <cassert>

namespace MMMEngine
{
	namespace Screen
	{
		inline void SetResolution(int width, int height) { assert(GlobalRegistry::Get().GetApp() && "글로벌 레지스트리에 Application이 등록되어있지 않습니다!");  GlobalRegistry::Get().GetApp()->SetWindowSize(width, height); }
		inline void SetResolution(int width, int height, DisplayMode mode)
		{ 
			assert(GlobalRegistry::Get().GetApp() && "글로벌 레지스트리에 Application이 등록되어있지 않습니다!");
			GlobalRegistry::Get().GetApp()->SetWindowSize(width, height);
			GlobalRegistry::Get().GetApp()->SetDisplayMode(mode);
		}

		inline std::vector<Resolution> GetResolutions() 
		{
			assert(GlobalRegistry::Get().GetApp() && "글로벌 레지스트리에 Application이 등록되어있지 않습니다!");
			return GlobalRegistry::Get().GetApp()->GetCurrentMonitorResolutions();
		}

		inline void SetResizable(bool isResizable)
		{
			assert(GlobalRegistry::Get().GetApp() && "글로벌 레지스트리에 Application이 등록되어있지 않습니다!");
			GlobalRegistry::Get().GetApp()->SetResizable(isResizable);
		}
	}
}
