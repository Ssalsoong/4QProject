#pragma once
#include "GlobalRegistry.h"
//#include "RenderManager.h"

#include "SimpleMath.h"
#include <cassert>

namespace MMMEngine
{
	namespace Screen
	{
		inline void SetResolution(int width, int height) { assert(g_pApp && "글로벌 레지스트리에 Application이 등록되어있지 않습니다!");  g_pApp->SetWindowSize(width, height); }
		inline void SetResolution(int width, int height, DisplayMode mode)
		{ 
			assert(g_pApp && "글로벌 레지스트리에 Application이 등록되어있지 않습니다!");  
			g_pApp->SetWindowSize(width, height); 
			g_pApp->SetDisplayMode(mode);
		}

		inline std::vector<Resolution> GetResolutions() 
		{
			assert(g_pApp && "글로벌 레지스트리에 Application이 등록되어있지 않습니다!");
			return g_pApp->GetCurrentMonitorResolutions();
		}

		inline void SetResizable(bool isResizable)
		{
			assert(g_pApp && "글로벌 레지스트리에 Application이 등록되어있지 않습니다!");
			g_pApp->SetResizable(isResizable);
		}
	}
}
