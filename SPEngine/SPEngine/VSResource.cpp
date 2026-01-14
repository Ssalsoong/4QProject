#include "VSResource.h"
#include "../BaseEngine/Helper.h"
#include "ResourceManager.h"

void VSResource::Initialize()
{
	if (!m_pVertexShader) {
		auto device = ResourceManager::GetInstance()->GetDevice();

		HR_T(CompileShaderFromFile(filePath.c_str(), "main", "vs_5_0", &m_pBlob));

		HR_T(device->CreateVertexShader(
			m_pBlob->GetBufferPointer(),
			m_pBlob->GetBufferSize(),
			nullptr,
			&m_pVertexShader
		));
	}
}
