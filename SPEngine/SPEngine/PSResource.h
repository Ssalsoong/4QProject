#pragma once
#include "GameResource.h"
#include "framework.h"

class PSResource : public GameResource
{
public:
	MW::ComPtr<ID3D11PixelShader> m_pPixelShader;
	MW::ComPtr<ID3D10Blob> m_pBlob;

	void Initialize();
};

