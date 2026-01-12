#pragma once
#include <Windows.h>
#include <string>

#include "Delegates.hpp"

namespace MMMEngine::Utility
{
	/// <summary>
	/// 윈도우 하나의 대한 핸들과 루프를 제공합니다.
	/// </summary>
	class App
	{
	public:
		struct WindowInfo
		{
			std::wstring title;
			LONG width;
			LONG height;
			LONG style;
		};

		App();
		App(LPCWSTR title, LONG width, LONG height);

		~App();

		int Run();
		void Quit();

		void SetTitle(const std::wstring& title);

		Event<App, void(void)> OnInitialize{ this };
		Event<App, void(void)> OnRelease{ this };
		Event<App, void(void)> OnUpdate{ this };
		Event<App, void(void)> OnRender{ this };
		Event<App, void(int,int)> OnWindowInfoChanged{ this };

		void SetProcessHandle(HINSTANCE hinstance);
		const WindowInfo GetWindowInfo() const;
		HWND GetWindowHandle() const;
	protected:
		LRESULT HandleWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		bool m_isRunning;
		WindowInfo m_windowInfo;
		
		HINSTANCE m_hInstance;
		HWND m_hWnd;

		bool m_windowInfoDirty;

		bool CreateMainWindow();
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}