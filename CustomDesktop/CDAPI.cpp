#include "stdafx.h"
#include <CDAPI.h>
#include "CDAPIModule.h"
#include "Global.h"
#include <CDEvents.h>


namespace cd
{
	CD_API HWND WINAPI GetTopHwnd()
	{
		return g_global.m_topWnd;
	}

	CD_API HWND WINAPI GetParentHwnd()
	{
		return g_global.m_parentWnd;
	}

	CD_API HWND WINAPI GetFileListHwnd()
	{
		return g_global.m_fileListWnd;
	}

	CD_API void WINAPI GetDesktopSize(SIZE& size)
	{
		size = g_global.m_wndSize;
	}

	CD_API void WINAPI GetScreenSize(SIZE& size)
	{
		size = g_global.m_screenSize;
	}

	CD_API void WINAPI RedrawDesktop(const RECT* rect)
	{
		RedrawWindow(g_global.m_fileListWnd, rect, NULL, RDW_ERASE | RDW_INVALIDATE);
	}


	CD_API UINT WINAPI GetFileListMsgID()
	{
		static UINT s_nextMsgID = WM_USER + 1000;
		return s_nextMsgID++;
	}

	CD_API UINT WINAPI GetParentMsgID()
	{
		static UINT s_nextMsgID = WM_USER + 1000;
		return s_nextMsgID++;
	}

	CD_API UINT WINAPI GetTopMsgID()
	{
		static UINT s_nextMsgID = WM_USER + 1000;
		return s_nextMsgID++;
	}

	CD_API UINT WINAPI GetMenuID()
	{
		static UINT s_nextMenuID = 100;
		return s_nextMenuID++;
	}


	CD_API std::wstring WINAPI GetPluginDir()
	{
		return g_global.m_cdDir + L"Plugin\\";
	}

	static const UINT WM_EXEC_FUNCTION = GetFileListMsgID();

	CD_API void WINAPI ExecInMainThread(std::function<void()> function)
	{
		PostMessage(g_global.m_fileListWnd, WM_EXEC_FUNCTION, reinterpret_cast<WPARAM>(
			new decltype(function)(std::move(function))), NULL);
	}


	// 实现ExecInMainThread
	static void OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& res, bool& pass)
	{
		if (message != WM_EXEC_FUNCTION)
			return;

		const auto function = reinterpret_cast<std::function<void()>*>(wParam);
		if (function != nullptr && *function)
		{
			(*function)();
			delete function;
			res = 1;
		}
		pass = false;
	}

	CDAPIModule::CDAPIModule()
	{
		g_fileListWndProcEvent.AddListener(OnFileListWndProc);
	}
}
