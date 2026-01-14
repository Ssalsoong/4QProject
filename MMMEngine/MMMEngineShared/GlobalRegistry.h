#pragma once
#include "Export.h"
#include "ExportSingleton.hpp"

namespace MMMEngine::Utility
{
	class App;
}

namespace MMMEngine
{
	class MMMENGINE_API GlobalRegistry : public Utility::ExportSingleton<GlobalRegistry>
	{
	private:
		Utility::App* m_pApp;

	public:
		void RegisterApp(Utility::App* app);
		void UnregisterApp();
		Utility::App* GetApp() const;
	};
}