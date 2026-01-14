#pragma once
#include "framework.h"
#include "Renderer.h"

class VSResource;
class PSResource;
class PhongRenderer : public Renderer
{
private:
	//MW::ComPtr<ID3D11VertexShader> m_pVertexShader;		// 정점 쉐이더
	//MW::ComPtr<ID3D11PixelShader> m_pAlphaClipShader;	// 디퓨즈 전용 쉐이더
	std::shared_ptr<VSResource> m_pVSShader;			// VS 리소스
	std::shared_ptr<PSResource> m_pPSShader;			// PS 리소스

	MW::ComPtr<ID3D11InputLayout> m_pInputLayout;		// 입력 레이아웃
	MW::ComPtr<ID3D11Buffer> m_pTransBuffer;	// 트랜스폼 버퍼
	MW::ComPtr<ID3D11Buffer> m_pMatBuffer;		// 메테리얼 버퍼
	MW::ComPtr<ID3D11Buffer> m_pBoneBuffer;		// 본 버퍼
	MW::ComPtr<ID3D11Buffer> m_pShadowBuffer;	// 그림자 버퍼
public:
	PhongRenderer();
	void Render() override;
};

