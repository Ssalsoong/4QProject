#include "InputManager.h"

using namespace DirectX::SimpleMath;

DEFINE_SINGLETON(MMMEngine::InputManager)

void MMMEngine::InputManager::StartUp(HANDLE windowHandle)
{
    m_hWnd = static_cast<HWND>(windowHandle); // 윈도우 핸들
}

void MMMEngine::InputManager::ShutDown()
{
    m_hWnd = NULL;
}

void MMMEngine::InputManager::Update()
{
    // 마우스 좌표
    ::GetCursorPos(&m_mouseClient);
    ::ScreenToClient(m_hWnd, &m_mouseClient);

    // 키보드 상태 업데이트
    memcpy_s(m_prevState, sizeof(m_prevState), m_currState, sizeof(m_currState));
    for (int i = 0; i < 256; i++) {
        m_currState[i] = GetAsyncKeyState(i);
    }


    //// 게임패드 상태 업데이트
    //for (auto& gamepad : m_gamepads)
    //{
    //    if (gamepad)
    //    {
    //        gamepad->Update();
    //    }
    //}
}


void MMMEngine::InputManager::HandleWindowResize(int newWidth, int newHeight)
{
    m_clientRect.right = newWidth;
    m_clientRect.bottom = newHeight;
    m_clientRect.left = 0;
    m_clientRect.top = 0;
}

Vector2 MMMEngine::InputManager::GetMousePos()
{
    return Vector2{ (float)m_mouseClient.x, (float)m_mouseClient.y };
}

bool MMMEngine::InputManager::GetKey(KeyCode keyCode)
{
    auto nativeKeyCode = static_cast<unsigned char>(keyCode);
    if (nativeKeyCode == -1) return false;
    return (m_currState[nativeKeyCode] & KEY_PRESSED) != 0;
}
bool MMMEngine::InputManager::GetKeyDown(KeyCode keyCode)
{
    auto nativeKeyCode = static_cast<unsigned char>(keyCode);
    if (nativeKeyCode == -1) return false;
    return (!(m_prevState[nativeKeyCode] & KEY_PRESSED) && (m_currState[nativeKeyCode] & KEY_PRESSED));
}
bool MMMEngine::InputManager::GetKeyUp(KeyCode keyCode)
{
    auto nativeKeyCode = static_cast<unsigned char>(keyCode);
    if (nativeKeyCode == -1) return false;
    return ((m_prevState[nativeKeyCode] & KEY_PRESSED) && !(m_currState[nativeKeyCode] & KEY_PRESSED));
}
