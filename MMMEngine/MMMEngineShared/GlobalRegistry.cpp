#include "GlobalRegistry.h"

DEFINE_SINGLETON(MMMEngine::GlobalRegistry)

void MMMEngine::GlobalRegistry::RegisterApp(Utility::App* app)
{
	m_pApp = app;
}

void MMMEngine::GlobalRegistry::UnregisterApp()
{
	m_pApp = nullptr;
}

MMMEngine::Utility::App* MMMEngine::GlobalRegistry::GetApp() const
{
	return m_pApp;
}
