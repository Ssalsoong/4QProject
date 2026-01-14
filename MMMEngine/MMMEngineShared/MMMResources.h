#pragma once
#include "ResourceManager.h"
#include <string>

namespace MMMEngine::Resources
{
	template <typename T>
	ResPtr<T> Load(const std::wstring& path) {
		return ResourceManager::Get().Load<T>(path);
	}
}