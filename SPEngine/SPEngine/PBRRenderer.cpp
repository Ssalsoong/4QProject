#include "PBRRenderer.h"
#include "ResourceManager.h"
#include "PSResource.h"
#include "VSResource.h"

PBRRenderer::PBRRenderer()
{
    // 리소스 불러오기
    m_pVSShader = ResourceManager::GetInstance()->LoadFile<VSResource>(L"../Resources/Shader/PBR/VS/SkeletalVertexShader.hlsl");
    m_pPSShader = ResourceManager::GetInstance()->LoadFile<PSResource>(L"../Resources/Shader/PBR/PS/BRDFShader.hlsl");

    // TODO::filePath 문제 다른방법 없나 찾아보기
    m_pVSShader->Initialize();
    m_pPSShader->Initialize();

}

void PBRRenderer::Render()
{

}

