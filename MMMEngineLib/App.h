#pragma once
#include <Windows.h>
#include <string>

#include "Delegates.h"

namespace MMMEngine
{
	class App
	{
	public:
		App(HINSTANCE hInstance);
		~App();
		bool Initialize();
		void Run();
		void Shutdown();
	protected:
		LRESULT HandleWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		HINSTANCE m_hInstance;
		HWND m_hWnd;
		std::wstring m_windowTitle;

		Action<> m_onIntialize;
		Action<> m_onShutdown;
		Action<> m_onUpdate;
		Action<> m_onRender;
		Action<> m_onResize;

		bool CreateMainWindow();
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}