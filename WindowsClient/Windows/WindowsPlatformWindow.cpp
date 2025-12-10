#include "WindowsPlatformWindow.h"
#include "Core/Engine.h"
#include "Input/RawInputHandler.h"
#include "Windows/KeyCodeMapping.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vector>

WindowsPlatformWindow::WindowsPlatformWindow(Engine* engine) : PlatformWindow(engine) {
    rawInputHandler = new RawInputHandler(engine);
}

WindowsPlatformWindow::~WindowsPlatformWindow() {
    Shutdown();
}

// Helper to convert UTF-16 (Windows) to UTF-8 (Engine)
std::string WideToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Helper to convert UTF-8 (Engine) to UTF-16 (Windows)
std::wstring Utf8ToWide(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}


// Global window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    WindowsPlatformWindow* window = (WindowsPlatformWindow*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (window != nullptr) {    
        switch (msg) {
            case WM_SIZE: {
                window->Resized.Fire();
                break;
            }
            case WM_CLOSE: {
                window->Closed.Fire();
                PostQuitMessage(0);
                break;
            }
            case WM_SETFOCUS: {
                window->Focused.Fire();
                break;
            }
            case WM_KILLFOCUS: {
                window->Unfocused.Fire();
                break;
            }
            case WM_CHAR: {
                wchar_t wch = static_cast<wchar_t>(wParam);
                if (wch >= 32) {
                    std::wstring wstr(1, wch);
                    window->ReceivedTextInput.Fire(WideToUtf8(wstr));
                }
                break;
            } /*
            case WM_KEYDOWN:
            case WM_KEYUP: {
                KeyCode key = Win32VirtualKeyToKeyCode(static_cast<UINT>(wParam));
                if (key != KeyCode::None) {
                    InputState state = (msg == WM_KEYDOWN) ? InputState::Begin : InputState::End;
                    window->KeyInputReceived.Fire(key, state);
                }
                return 0;
            }*/
            case WM_DESTROY: {
                break;
            }
            case WM_INPUT: {
                window->ProcessRawInput(lParam);
                break;
            }
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void WindowsPlatformWindow::ProcessRawInput(LPARAM lParam) {
    if (rawInputHandler) {
        rawInputHandler->HandleInput(lParam);
    }
}

bool WindowsPlatformWindow::Initialize(const std::string& title, int width, int height) {
    std::wstring wtitle = Utf8ToWide(title);
    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, wtitle.c_str(), NULL };
    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowW(wtitle.c_str(), wtitle.c_str(), WS_OVERLAPPEDWINDOW, 100, 100, width, height, NULL, NULL, wc.hInstance, NULL);
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)this);

    if (!hwnd) {
        return false;
    }

    windowHandle = hwnd;
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    rawInputHandler->Initialize(this);

    return true;
}

void WindowsPlatformWindow::Shutdown() {
    if (windowHandle) {
        DestroyWindow((HWND)windowHandle);
        windowHandle = nullptr;
    }
}

bool WindowsPlatformWindow::PollEvents() {
    MSG msg;
    while (PeekMessageW(&msg, NULL, 0U, 0U, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
        
        if (msg.message == WM_QUIT) {
            return false;
        }
    }
    return true;
}

void* WindowsPlatformWindow::GetNativeWindowHandle() const {
    return windowHandle;
}

Math::Rect<int> WindowsPlatformWindow::GetInternalBounds() const {
    if (!windowHandle) {
        return Math::Rect<int>(0, 0, 0, 0);
    }

    RECT rect;
    GetClientRect((HWND)windowHandle, &rect);
    return Math::Rect<int>(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
}

Math::Rect<int> WindowsPlatformWindow::GetExternalBounds() const {
    if (!windowHandle) {
        return Math::Rect<int>(0, 0, 0, 0);
    }

    RECT rect;
    GetWindowRect((HWND)windowHandle, &rect);
    return Math::Rect<int>(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
}