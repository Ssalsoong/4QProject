#pragma once
#include "GameResource.h"
#include "framework.h"

class VSResource : public GameResource
{
public:
	MW::ComPtr<ID3D11VertexShader> m_pVertexShader;
	MW::ComPtr<ID3D10Blob> m_pBlob;

	void Initialize();
};

