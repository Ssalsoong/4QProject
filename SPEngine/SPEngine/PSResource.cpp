#include "PSResource.h"
#include "ResourceManager.h"
#include "../BaseEngine/Helper.h"

void PSResource::Initialize()
{
	if (!m_pPixelShader) {
		auto device = ResourceManager::GetInstance()->GetDevice();

		HR_T(CompileShaderFromFile(filePath.c_str(), "main", "ps_5_0", &m_pBlob));

		HR_T(device->CreatePixelShader(
			m_pBlob->GetBufferPointer(),
			m_pBlob->GetBufferSize(),
			nullptr,
			&m_pPixelShader
		));
	}
}
