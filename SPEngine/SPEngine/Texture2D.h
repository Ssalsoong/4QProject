#pragma once
#include "framework.h"
#include "GameResource.h"

class Texture2D : public GameResource
{
public:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv = nullptr;
};

