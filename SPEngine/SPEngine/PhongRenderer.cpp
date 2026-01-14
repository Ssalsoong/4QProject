#include "PhongRenderer.h"
#include "RenderPipe.h"
#include "ResourceManager.h"
#include "PSResource.h"
#include "VSResource.h"
#include "../BaseEngine/Helper.h"
#include "ResourceShared.h"
#include "PhongMaterial.h"

PhongRenderer::PhongRenderer()
{
	// 리소스 불러오기
	m_pVSShader = ResourceManager::GetInstance()->LoadFile<VSResource>(L"../Resources/Shader/B_Phong/VS/SkeletalVertexShader.hlsl");
	m_pPSShader = ResourceManager::GetInstance()->LoadFile<PSResource>(L"../Resources/Shader/B_Phong/PS/AlphaClipPixelShader.hlsl");
	
	// TODO::filePath 문제 다른방법 없나 찾아보기
	m_pVSShader->Initialize();
	m_pPSShader->Initialize();

	auto device = ResourceManager::GetInstance()->GetDevice();

	// 인풋 레이아웃 생성
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEINDEX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	HR_T(device->CreateInputLayout(
		layout, ARRAYSIZE(layout), m_pVSShader->m_pBlob->GetBufferPointer(),
		m_pVSShader->m_pBlob->GetBufferSize(), &m_pInputLayout
	));

	auto m_pDevice = ResourceManager::GetInstance()->GetDevice();

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	
	bd.ByteWidth = sizeof(Render_TransformBuffer);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pTransBuffer));

	bd.ByteWidth = sizeof(Render_MaterialBuffer);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pMatBuffer));

	bd.ByteWidth = sizeof(Mesh_BoneBuffer);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pBoneBuffer));

	bd.ByteWidth = sizeof(Render_ShadowBuffer);
	HR_T(m_pDevice->CreateBuffer(&bd, nullptr, &m_pShadowBuffer));
}

void PhongRenderer::Render()
{
	
	// 버텍스 등록
	UINT stride = sizeof(Mesh_Vertex);
	UINT offset = 0;
	m_pDeviceContext->IASetInputLayout(m_pInputLayout.Get());
	m_pDeviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	m_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, offset);

	m_pDeviceContext->VSSetShader(m_pVSShader->m_pVertexShader.Get(), nullptr, 0);
	m_pDeviceContext->PSSetShader(m_pPSShader->m_pPixelShader.Get(), nullptr, 0);

	// 트랜스폼 등록
	Render_TransformBuffer transformBuffer;
	transformBuffer.mWorld = XMMatrixTranspose(m_worldMat);
	transformBuffer.mNormalMatrix = XMMatrixInverse(nullptr, m_worldMat);
	m_pDeviceContext->UpdateSubresource1(m_pTransBuffer.Get(), 0, nullptr, &transformBuffer, 0, 0, D3D11_COPY_DISCARD);
	m_pDeviceContext->VSSetConstantBuffers(1, 1, m_pTransBuffer.GetAddressOf());
	//m_pDeviceContext->PSSetConstantBuffers(1, 1, m_pTransBuffer.GetAddressOf());

	// 메테리얼 등록
	auto material = dynamic_cast<PhongMaterial*>(m_pMaterial.get());
	if (material) {
		Render_MaterialBuffer matBuffer;
		matBuffer.Matambient = material->baseColor;
		matBuffer.Matdiffuse = material->matDiffuse;
		matBuffer.Matspecular = material->matSpecular;
		matBuffer.shiness = material->shiness;
		m_pDeviceContext->UpdateSubresource1(m_pMatBuffer.Get(), 0, nullptr, &matBuffer, 0, 0, D3D11_COPY_DISCARD);
		//m_pDeviceContext->VSSetConstantBuffers(3, 1, m_pMatBuffer.GetAddressOf());
		m_pDeviceContext->PSSetConstantBuffers(3, 1, m_pMatBuffer.GetAddressOf());

		ID3D11ShaderResourceView* rsv[4] = {
			material->diffuse ? material->diffuse->srv.Get() : nullptr,
			material->normal ? material->normal->srv.Get() : nullptr,
			material->specular ? material->specular->srv.Get() : nullptr,
			material->emissive ? material->emissive->srv.Get() : nullptr
		};
		m_pDeviceContext->PSSetShaderResources(0, ARRAYSIZE(rsv), rsv);
	}



	// 드로우콜
	m_pDeviceContext->DrawIndexed(m_IndicesSize, 0, 0);
}
