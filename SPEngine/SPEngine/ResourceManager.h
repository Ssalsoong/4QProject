#pragma once
#include "framework.h"
#include <memory>
#include <unordered_map>
#include <string>
#include "GameResource.h"

class ResourceManager
{
private:
	std::unordered_map<std::wstring, std::shared_ptr<GameResource>> resources;
	std::vector<std::wstring> erasePending;
	static std::unique_ptr<ResourceManager> instance;
    Microsoft::WRL::ComPtr<ID3D11Device5> m_pDevice;

public:
	static ResourceManager* GetInstance();
	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device5>& _device);
    virtual void Start();
	void Update();
	
	ID3D11Device5* GetDevice() { return m_pDevice.Get(); }

	template <typename T, typename ... Args>
	std::shared_ptr<T> LoadFile(std::wstring _path, Args&&... args);
};

template <typename T, typename ... Args>
std::shared_ptr<T>
ResourceManager::LoadFile(std::wstring _path, Args&&... args)
{
	auto it = resources.find(_path);
	
	if (it != resources.end()) {
		// 기존 리소스 반환, 만료시간 초기화
		it->second->elipsedTime = 0.0f;
		return std::dynamic_pointer_cast<T>(it->second);
	}
	else {
		// 새로운 리소스 생성
		auto resource = std::make_shared<T>(std::forward<Args>(args)...);
		resource->filePath = _path;
		resources[_path] = resource;
		return resource;
	}
		
}

