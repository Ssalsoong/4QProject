#include "Behaviour.h"
#include "BehaviourManager.h"

MMMEngine::Behaviour::Behaviour() 
	: m_enabled(true)
{
	
}

void MMMEngine::Behaviour::Initialize()
{
	BehaviourManager::Get().RegisterBehaviour(SelfPtr(this));
}

void MMMEngine::Behaviour::UnInitialize()
{
	BehaviourManager::Get().UnRegisterBehaviour(SelfPtr(this)); // BehaviourManager에서 제거
	m_messages.clear();
}

void MMMEngine::Behaviour::SetEnabled(bool value)
{
	if (value != m_enabled)
	{
		//바뀔 때 무언갈 실행하는 코드 작성하기
		m_enabled = value;
	}
}

bool MMMEngine::Behaviour::IsActiveAndEnabled()
{
	return m_enabled && GetGameObject().IsValid() && GetGameObject()->IsActiveInHierarchy();
}
