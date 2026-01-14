#pragma once
#include "Renderer.h"
#include "framework.h"

class SkyBoxRenderer : public Renderer
{
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pSkyVertexShader = nullptr;		// 스카이박스 정점 쉐이더
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pSkyPixelShader = nullptr;			// 스카이박스 픽셀 쉐이더
	
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pSkyInputLayout = nullptr;			// 스카이박스입력 레이아웃
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_SkyboxRS = nullptr;				// 스카이박스 전용 RS
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pSkyTextureRV = nullptr;		// 스카이박스 텍스처 리소스 뷰.
};

