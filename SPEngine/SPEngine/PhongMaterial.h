#pragma once
#include "Material.h"
#include "Texture2D.h"

class PhongMaterial : public Material
{
public:
	std::shared_ptr<Texture2D> diffuse = nullptr;		// 디퓨즈맵
	std::shared_ptr<Texture2D> specular = nullptr;		// 스펙큘러맵
	std::shared_ptr<Texture2D> normal = nullptr;		// 노멀맵
	std::shared_ptr<Texture2D> emissive = nullptr;		// 발광맵

	DirectX::SimpleMath::Vector4 matDiffuse = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::SimpleMath::Vector4 matSpecular = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	UINT shiness = 32;
};

