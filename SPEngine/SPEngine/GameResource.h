#pragma once
#include <string>

class ResourceManager;
class GameResource
{
	friend class ResourceManager;
public:
	virtual ~GameResource() = default;
	std::wstring filePath;
private:
	float elipsedTime = 0.0f;
};

