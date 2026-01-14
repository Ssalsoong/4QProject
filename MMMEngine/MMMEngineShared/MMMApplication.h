#pragma once
#include "GlobalRegistry.h"
#include <cassert>
#include <string>

namespace MMMEngine::Application
{
	inline void Quit() { assert(GlobalRegistry::Get().GetApp() && "글로벌 레지스트리에 Application이 등록되어있지 않습니다!"); GlobalRegistry::Get().GetApp()->Quit(); }

	inline void SetWindowTitle(const std::wstring& title) { assert(GlobalRegistry::Get().GetApp() && "글로벌 레지스트리에 Application이 등록되어있지 않습니다!"); GlobalRegistry::Get().GetApp()->SetWindowTitle(title); }
}