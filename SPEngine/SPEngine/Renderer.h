#pragma once
#include "framework.h"

class Material;
class Renderer
{
protected:
	bool isUsable = false;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
	UINT m_IndicesSize;
	std::shared_ptr<Material> m_pMaterial;

protected:
	MW::ComPtr<ID3D11DeviceContext4> m_pDeviceContext;
	DirectX::SimpleMath::Matrix m_worldMat;

public:
	Renderer();

	void SetWorldMat(const DirectX::SimpleMath::Matrix& _mat);

	void SetRenderData(
		Microsoft::WRL::ComPtr<ID3D11Buffer>& _vertex,
		Microsoft::WRL::ComPtr<ID3D11Buffer>& _index,
		UINT _indicesSize,
		std::shared_ptr<Material>& _material
	);
	virtual void Render() = 0;
};

