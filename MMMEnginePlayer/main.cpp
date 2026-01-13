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

using namespace MMMEngine;
using namespace MMMEngine::Utility;

ObjPtr<GameObject> g_pPlayer = nullptr;

void Initialize()
{
	InputManager::Get().StartUp(GlobalRegistry::Get().GetApp()->GetWindowHandle());
	GlobalRegistry::Get().GetApp()->OnWindowSizeChanged.AddListener<InputManager, &InputManager::HandleWindowResize>(&InputManager::Get());

	BehaviourManager::Get().StartUp();

	g_pPlayer = Object::NewObject<GameObject>("Player");

	Object::Destroy(g_pPlayer,5.0f);
}

void Update()
{
	TimeManager::Get().BeginFrame();
	InputManager::Get().Update();

	BehaviourManager::Get().InitializeBehaviours();
	
	//씬 변환 후 한번만 호출
	//BehaviourManager::Get().AllSortBehaviours();
	//BehaviourManager::Get().AllBroadCastBehaviourMessage("OnSceneLoaded");

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
	BehaviourManager::Get().DisableBehaviours();
	ObjectManager::Get().ProcessPendingDestroy();
}

void Render()
{

}

void Release()
{
	BehaviourManager::Get().ShutDown();
	GlobalRegistry::Get().UnregisterApp();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	App app{ hInstance,L"MMMPlayer",1280,720 };
	GlobalRegistry::Get().RegisterApp(&app);

	app.OnInitialize.AddListener<&Initialize>();
	app.OnUpdate.AddListener<&Update>();
	app.OnRender.AddListener<&Render>();
	app.OnRelease.AddListener<&Release>();
	app.Run();
}