#include "PBRApp.h"
#include "ResourceManager.h"
#include "RenderPipe.h"
#include "../BaseEngine/Helper.h"
#include "AssimpLoader.h"
#include "ObjectManager.h"
#include "MeshRenderer.h"
#include "GameObject.h"
#include "Transform.h"

using namespace Microsoft;
using namespace Microsoft::WRL;

Microsoft::WRL::ComPtr<ID3D11Device5> PBRApp::m_pDevice;
Camera* PBRApp::mainCamera = nullptr;

PBRApp::~PBRApp()
{
}

bool PBRApp::Initialize(UINT Width, UINT Height)
{
	__super::Initialize(Width, Height);

	// 카메라 인스턴스 등록
	mainCamera = &m_Camera;

	// 디바이스 생성
	ComPtr<ID3D11Device> device;
	D3D_FEATURE_LEVEL featureLevel;
	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, 
		0, nullptr, 0, D3D11_SDK_VERSION,
		device.GetAddressOf(), &featureLevel, nullptr);

	HR_T(device.As(&m_pDevice));

	// 객체 초기화
	ResourceManager::GetInstance()->Initialize(m_pDevice);
	RenderPipe::GetInstance()->Initialize(m_pDevice, &m_hWnd, m_ClientWidth, m_ClientHeight, mainCamera);
	AssimpLoader::GetInstance()->Initialize();
	GameTimer::m_Instance->Reset();
	GameTimer::m_Instance->Start();

	// 씬 초기화
	if (!InitScene())
		return false;

	return true;
}

void PBRApp::Update()
{
	__super::Update();
	ResourceManager::GetInstance()->Update();
	ObjectManager::GetInstance()->Update();
}

void PBRApp::Render()
{
	RenderPipe::GetInstance()->Render();
}

bool PBRApp::InitScene()
{
	// 젤다 오브젝트 생성
	auto zelda = ObjectManager::GetInstance()->CreateObject();
	auto zmesh = AssimpLoader::GetInstance()->LoadStaticMesh(L"../Resources/Models/Zelda/Zelda.fbx");
	auto zmc = zelda.lock()->AddComponent<MeshRenderer>();
	zmc.lock()->SetMesh(zmesh);
	zelda.lock()->name = L"ZeldaMesh";
	zelda.lock()->transform.lock()->Position = Vector3(0.0f, 0.0f, 0.0f);
	zelda.lock()->transform.lock()->Scale = Vector3(0.1f, 0.1f, 0.1f);

	return true;
}

void PBRApp::UninitScene()
{
	
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK PBRApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return __super::WndProc(hWnd, message, wParam, lParam);
}
