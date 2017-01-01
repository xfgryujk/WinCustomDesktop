#pragma once
#include "CDCommon.h"
#include <functional>
#include <string>


namespace cd
{
	CD_API HWND WINAPI GetTopHwnd();
	CD_API HWND WINAPI GetParentHwnd();
	CD_API HWND WINAPI GetFileListHwnd();
	CD_API void WINAPI GetDesktopSize(SIZE& size);
	CD_API void WINAPI GetScreenSize(SIZE& size);
	CD_API void WINAPI RedrawDesktop(const RECT* rect = NULL);

	CD_API std::wstring WINAPI GetPluginDir();
	// 在FileList窗口过程中分配一个唯一的消息ID
	CD_API UINT WINAPI GetCustomMessageID();
	// 在主线程中执行函数，可以用来做dllmain中不能完成的初始化，通过自定义消息实现
	CD_API void WINAPI ExecInMainThread(std::function<void()> function);
}
