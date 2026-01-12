#define NOMINMAX
#include <iostream>

#include "GlobalRegistry.h"
#include "PlayerRegistry.h"
#include "App.h"

#include "MMMApplication.h"
#include "MMMScreen.h"
#include "MMMTime.h"
#include "MMMInput.h"

#include "InputManager.h"
#include "TimeManager.h"
#include "ResourceManager.h"
#include "BehaviourManager.h"
#include "ObjectManager.h"

#include "PlayerMove.h"

using namespace MMMEngine;
using namespace MMMEngine::Utility;

ObjPtr<GameObject> g_pPlayer = nullptr;

void Initialize()
{
	InputManager::Get().StartUp(g_pApp->GetWindowHandle());
	g_pApp->OnWindowSizeChanged.AddListener<InputManager, &InputManager::HandleWindowResize>(&InputManager::Get());

	ResourceManager::Get().SetResolver(&Player::g_resolver);
	ResourceManager::Get().SetBytesProvider(&Player::g_bytes);
	BehaviourManager::Get().StartUp();

	g_pPlayer = Object::NewObject<GameObject>("Player");
	g_pPlayer->AddComponent<PlayerMove>();
}

void Update()
{
	InputManager::Get().Update();
	TimeManager::Get().BeginFrame();

	BehaviourManager::Get().InitializeBehaviours();
	//BehaviourManager::Get().AllSortBehaviours();

	float dt = TimeManager::Get().GetDeltaTime();

	TimeManager::Get().ConsumeFixedSteps([&](float fixedDt)
	{
		//PhysicsManager::Get()->PreSyncPhysicsWorld();
		//PhysicsManager::Get()->PreApplyTransform();
		BehaviourManager::Get().BroadCastBehaviourMessage("FixedUpdate");
		//PhysicsManager::Get()->Simulate(fixedDt);
		//PhysicsManager::Get()->ApplyTransform();
	});

	BehaviourManager::Get().BroadCastBehaviourMessage("Update");

	BehaviourManager::Get().BroadCastBehaviourMessage("LateUpdate");

	ObjectManager::Get().UpdateInternalTimer(dt);
	ObjectManager::Get().ProcessPendingDestroy();
}

void Render()
{

}

void Release()
{
	BehaviourManager::Get().ShutDown();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	//if (!Player::g_pathMap.LoadFromFile(L"Data/PathMap.bin"))
	//	return -1;

	//if (!Player::g_assetIndex.LoadFromFile(L"Data/AssetIndex.bin"))
	//	return -2;

	//if (!Player::g_pak.Open(L"Data/assets.pak"))
	//	return -3;

	App app{ hInstance,L"MMMPlayer",1280,720 };
	g_pApp = &app;

	app.OnInitialize.AddListener<&Initialize>();
	app.OnUpdate.AddListener<&Update>();
	app.OnRender.AddListener<&Render>();
	app.OnRelease.AddListener<&Release>();
	app.Run();
}