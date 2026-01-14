#include "Renderer.h"
#include "Material.h"
#include "RenderPipe.h"

Renderer::Renderer()
{
	m_IndicesSize = 0;
	m_pDeviceContext = RenderPipe::GetInstance()->m_pDeviceContext;
}

void Renderer::SetWorldMat(const DirectX::SimpleMath::Matrix& _mat)
{
	m_worldMat = _mat;
}

void Renderer::SetRenderData(Microsoft::WRL::ComPtr<ID3D11Buffer>& _vertex, Microsoft::WRL::ComPtr<ID3D11Buffer>& _index, UINT _indicesSize, std::shared_ptr<Material>& _material)
{
	// 인풋값 유효성 체크
	if (!_vertex || !_index)
		return;

	m_pVertexBuffer = _vertex;
	m_pIndexBuffer = _index;
	m_IndicesSize = _indicesSize;
	m_pMaterial = _material;
}

