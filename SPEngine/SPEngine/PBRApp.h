#pragma once
#include "../BaseEngine/GameApp.h"

#include <imgui.h>
#include <wrl/client.h>
#include <directxtk/SimpleMath.h>
#include <string>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <Psapi.h>
#include <memory>

#include "GameObject.h"
#include "framework.h"
#include "RenderStruct.h"

using namespace Microsoft::WRL;

class PBRApp : public GameApp
{
public:
	static Microsoft::WRL::ComPtr<ID3D11Device5> m_pDevice;
	static Camera* mainCamera;

	std::vector<GameObject> objects;

public:
	PBRApp(HINSTANCE hInstance) : GameApp(hInstance) {};
	~PBRApp();

	bool Initialize(UINT Width, UINT Height) override;
	void Update() override;
	void Render() override;

	bool InitScene();		// Ω¶¿Ã¥ı,πˆ≈ÿΩ∫,¿Œµ¶Ω∫
	void UninitScene();

public:
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

