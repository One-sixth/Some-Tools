#include <Windows.h>

// 全局钩子版

//#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
//#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
//
//HHOOK hook;
//
//LRESULT CALLBACK callnexthook(int nCode, WPARAM wParam, LPARAM lParam)
//{
//	if (KEYDOWN(VK_F2) && KEYDOWN(VK_RCONTROL))
//	{
//		HWND hwnd = GetForegroundWindow();
//		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOMOVE | SWP_NOSIZE);
//	}
//	else if (KEYDOWN(VK_F3) && KEYDOWN(VK_RCONTROL))
//	{
//		HWND hwnd = GetForegroundWindow();
//		SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOMOVE | SWP_NOSIZE);
//	}
//	if (KEYDOWN(VK_F12) && KEYDOWN(VK_RCONTROL))
//	{
//		PostQuitMessage(0);
//	}
//	return CallNextHookEx(0, nCode, wParam, lParam);
//}
//
//int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
//{
//	char *helpText = "RCtrl+F2 置顶窗口\nRCtrl+F3 取消置顶窗口\nRCtrl+F12 退出程序\n";
//	MessageBox(0, helpText, "Help Text", 0);
//	hook = SetWindowsHookExA(WH_KEYBOARD_LL, callnexthook, hInstance, 0);
//	if (hook = 0)
//		return 0;
//	MSG msg;
//	while (int r = GetMessage(&msg, 0, 0, 0) && r != -1)
//	{
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//	}
//	UnhookWindowsHookEx(hook);
//	return 0;
//}


// 全局热键版

ATOM hotKeyId1, hotKeyId2;

LRESULT __stdcall WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_HOTKEY:
		if (wParam == hotKeyId1)
			PostMessageA(0, WM_USER + 1, 0, 0);
		else if (wParam == hotKeyId2)
			PostMessageA(0, WM_USER + 2, 0, 0);
		break;
	}
	return DefWindowProcA(hWnd, Msg, wParam, lParam);
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// 全局唯一
	auto m = CreateMutexA(NULL, TRUE, "{F6C8A0A9-0916-4BB6-B300-2DC5418593D6}");

	if (ERROR_ALREADY_EXISTS == GetLastError())
	{
		MessageBox(0, "程序已在运行", "Error", 0);
		return -1;
	}

	char *helpText = "Ctrl+F2 置顶当前激活窗口\nCtrl+F3 取消置顶当前激活窗口\n结束程序请在任务管理器强制结束\n";
	MessageBox(0, helpText, "Help Text", 0);

	//创建隐藏窗口，仅用于捕获热键
	WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(wcex);
	wcex.style = 0;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandleA(0);
	wcex.hIcon = 0;
	wcex.hCursor = 0;
	wcex.hbrBackground = 0;
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = "test";
	if (RegisterClassExA(&wcex) == FALSE)
		return -1;
	HWND hwnd = CreateWindowA("test", "", WS_EX_NOACTIVATE, 0, 0, 0, 0, 0, 0, GetModuleHandleA(0), 0);
	ShowWindow(hwnd, SW_HIDE);

	//注册热键
	hotKeyId1 = GlobalAddAtom("TopWindow") - 0xc000;
	hotKeyId2 = GlobalAddAtom("NoTopWindow") - 0xc000;
	int b = RegisterHotKey(hwnd, hotKeyId1, MOD_CONTROL, VK_F2);
	b = RegisterHotKey(hwnd, hotKeyId2, MOD_CONTROL, VK_F3);

	MSG msg;
	while (GetMessageA(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
		if (msg.message == WM_USER + 1)
		{
			HWND hwnd = GetForegroundWindow();
			SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOMOVE | SWP_NOSIZE);
		}
		else if (msg.message == WM_USER + 2)
		{
			HWND hwnd = GetForegroundWindow();
			SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOMOVE | SWP_NOSIZE);
		}
	}
	// 一般不会到达这里，一般是直接任务管理器结束此程序
	// 清理
	UnregisterHotKey(hwnd, hotKeyId1);
	UnregisterHotKey(hwnd, hotKeyId2);
	GlobalDeleteAtom(hotKeyId1);
	GlobalDeleteAtom(hotKeyId2);
	return 0;
}