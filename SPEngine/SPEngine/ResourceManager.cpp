#include "ResourceManager.h"
#include "../BaseEngine/TimeSystem.h"
using namespace std;

#ifdef _DEBUG
	#define OUTDATETIME 10.0f
#else
	#define OUTDATETIME 300.0f
#endif // _DEBUG
#define TICKRATE 5.0f

std::unique_ptr<ResourceManager> ResourceManager::instance = nullptr;

ResourceManager* ResourceManager::GetInstance()
{
	if (!instance) {
		instance = std::make_unique<ResourceManager>();
	}
	return instance.get();
}

void ResourceManager::Initialize(Microsoft::WRL::ComPtr<ID3D11Device5>& _device)
{
	// 디바이스 등록
	assert(_device && "ResourceManager::Initialize : device must not be nullptr!!");
	m_pDevice = _device;

	// 객체 초기화 함수
	instance->Start();
	resources.clear();
}

void ResourceManager::Start()
{
	
}

void ResourceManager::Update()
{
	static float tickTime = 0.0f;
	if (tickTime > TICKRATE) {
		for (const auto& resource : resources) {
			if (resource.second.use_count() == 1) {
				if (resource.second->elipsedTime >= OUTDATETIME)
					erasePending.push_back(resource.first);
				else
					resource.second->elipsedTime += tickTime;
			}
			else
				resource.second->elipsedTime = 0.0f;
		}

		if (!erasePending.empty()) {
			for (auto& key : erasePending)
				resources.erase(key);
			erasePending.clear();
		}

		while(tickTime > TICKRATE)
			tickTime -= TICKRATE;
	}
	else {
		tickTime += GameTimer::m_Instance->DeltaTime();
	}
}
