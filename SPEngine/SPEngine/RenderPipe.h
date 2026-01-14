#pragma once
#include "framework.h"
#include "Renderer.h"
#include <vector>
#include <memory>
#include <map>
#include "RenderStruct.h"

class Camera;
class RenderPipe
{
	friend class Renderer;
private:
	RenderPipe() = default;
	//std::vector<std::shared_ptr<Renderer>> renderers;
	std::map<int, std::vector<std::shared_ptr<Renderer>>> passes;
	static std::unique_ptr<RenderPipe> instance;

protected:
	// 외부 인터페이스
	Microsoft::WRL::ComPtr<ID3D11Device5> m_pDevice;	// 디바이스 포인터
	HWND* m_pHwnd = nullptr;	// HWND 포인터
	UINT m_rClientWidth = 0;
	UINT m_rClientHeight = 0;
	float m_backColor[4] = { 0.0f, 0.5f, 0.5f, 1.0f };	// 백그라운드 컬러

	// 기본 렌더 인터페이스
	Microsoft::WRL::ComPtr<ID3D11DeviceContext4> m_pDeviceContext;		// 디바이스 컨텍스트
	Microsoft::WRL::ComPtr<IDXGISwapChain4> m_pSwapChain;				// 스왑체인

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView1> m_pRenderTargetView;	// 렌더링 타겟뷰
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;		// 깊이값 처리를 위한 뎊스스텐실 뷰

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pDafaultSamplerLinear;		// 샘플러 상태.
	Microsoft::WRL::ComPtr<ID3D11RasterizerState2> m_pDefaultRS;			// 기본 RS

	Microsoft::WRL::ComPtr<ID3D11BlendState1> m_pDefaultBS;		// 기본 블랜드 스테이트
	Microsoft::WRL::ComPtr<ID3D11RasterizerState2> m_DefaultRS;	// 기본 레스터라이저 스테이트
	D3D11_VIEWPORT m_defaultViewport;							// 기본 뷰포트

	// 버퍼 기본색상
	DirectX::SimpleMath::Vector4 m_ClearColor;

	// 카메라 관련
	Camera* m_renderCam = nullptr;
	Render_CamBuffer m_camMat;
	MW::ComPtr<ID3D11Buffer> m_pCambuffer = nullptr;		// 트랜스폼 버퍼

public:
	static RenderPipe* GetInstance();
	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device5>& _device, HWND* _hwnd, UINT _ClientWidth, UINT _ClientHeight, Camera* _renderCam);
	void InitD3D();
	void UnInitD3D();
	void Start();
	void Render();
	void SetRenderCam(Camera* _renderCam) { m_renderCam = _renderCam; }

public:
	template <typename T, typename... Args>
	std::weak_ptr<Renderer> AddRenderer(RenderType _passType, Args&&... args);

	template <typename T>
	bool RemoveRenderer(RenderType _passType, std::shared_ptr<T>& _renderer);
};

template <typename T, typename... Args>
std::weak_ptr<Renderer>
RenderPipe::AddRenderer(RenderType _passType, Args&&... args)
{
	std::shared_ptr<T> temp = std::make_shared<T>(std::forward<Args>(args)...);
	passes[_passType].push_back(temp);

	return temp;
}

template <typename T>
bool RenderPipe::RemoveRenderer(RenderType _passType, std::shared_ptr<T>& _renderer)
{
	if (_renderer && (passes.find(_passType) != passes.end())) {
		auto it = std::find(passes[_passType].begin(), passes[_passType].end(), _renderer);

		if (it != passes[_passType].end()) {
			passes[_passType].erase(it);
			return true;
		}
	}
	
	return false;
}

