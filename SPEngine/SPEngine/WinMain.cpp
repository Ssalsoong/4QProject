#include "../BaseEngine/pch.h"
#include "PBRApp.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	PBRApp App(hInstance);  // 생성자에서 아이콘,윈도우 이름만 바꾼다
	if (!App.Initialize(1280, 720))
		return -1;

	return App.Run();
}