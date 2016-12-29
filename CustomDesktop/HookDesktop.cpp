#include "stdafx.h"
#include "HookDesktop.h"
#include "DesktopInfo.h"
#include "InternalEvents.h"


namespace cd
{
	// 初始化，寻找窗口句柄
	bool HookDesktop::Init()
	{
		return Init(g_desktopInfo.m_fileListWnd);
	}

	// 初始化，子类化窗口，hook
	bool HookDesktop::Init(HWND fileListWnd)
	{
		if (m_hasInit)
			return true;
		if (!IsWindow(fileListWnd))
			return false;
		if (GetModuleHandle(_T("explorer.exe")) == NULL)
			return false;

		// 子类化
		m_oldFileListWndProc = (WNDPROC)SetWindowLongPtr(g_desktopInfo.m_fileListWnd, GWLP_WNDPROC, (ULONG_PTR)FileListWndProc);
		if (m_oldFileListWndProc == NULL)
			return false;
		m_oldParentWndProc = (WNDPROC)SetWindowLongPtr(g_desktopInfo.m_parentWnd, GWLP_WNDPROC, (ULONG_PTR)ParentWndProc);
		if (m_oldParentWndProc == NULL)
			return false;

		// hook
		m_beginPaintHook.Enable();
		m_endPaintHook.Enable();

		m_hasInit = true;
		return true;
	}

	// 释放，貌似在不同线程，运气不好可能会崩溃...懒得加锁了
	bool HookDesktop::Uninit()
	{
		if (!m_hasInit)
			return true;
		m_hasInit = false;

		if (IsWindow(g_desktopInfo.m_fileListWnd) && m_oldFileListWndProc != NULL)
			SetWindowLongPtr(g_desktopInfo.m_fileListWnd, GWLP_WNDPROC, (ULONG_PTR)m_oldFileListWndProc);
		if (IsWindow(g_desktopInfo.m_parentWnd) && m_oldParentWndProc != NULL)
		{
			SetWindowLongPtr(g_desktopInfo.m_parentWnd, GWLP_WNDPROC, (ULONG_PTR)m_oldParentWndProc);
			if (IsWindow(g_desktopInfo.m_fileListWnd))
				InvalidateRect(g_desktopInfo.m_fileListWnd, NULL, TRUE);
		}

		m_endPaintHook.Disable();
		m_beginPaintHook.Disable();

		m_oldFileListWndProc = m_oldParentWndProc = NULL;

		return true;
	}

	HookDesktop::HookDesktop() :
		m_beginPaintHook(GetModuleHandle(_T("comctl32.dll")), "user32.dll", "BeginPaint", MyBeginPaint, false),
		m_endPaintHook(GetModuleHandle(_T("comctl32.dll")), "user32.dll", "EndPaint", MyEndPaint, false)
	{
		Init();
	}

	HookDesktop::~HookDesktop()
	{
		Uninit();
	}


	// 静态文件列表窗口过程，传递给动态文件列表窗口过程
	LRESULT CALLBACK HookDesktop::FileListWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return HookDesktop::GetInstance().OnFileListWndProc(hwnd, message, wParam, lParam);
	}

	// 静态父窗口过程，传递给动态父窗口过程
	LRESULT CALLBACK HookDesktop::ParentWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return HookDesktop::GetInstance().OnParentWndProc(hwnd, message, wParam, lParam);
	}

	// 静态BeginPaint的hook，传递给动态的OnBeginPaint方法
	HDC WINAPI HookDesktop::MyBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint)
	{
		auto& instance = HookDesktop::GetInstance();
		if (hWnd == g_desktopInfo.m_fileListWnd)
			return instance.OnBeginPaint(hWnd, lpPaint);
		return instance.m_beginPaintHook.m_oldEntry(hWnd, lpPaint);
	}

	// 静态EndPaint的hook，传递给动态的OnEndPaint方法
	BOOL WINAPI HookDesktop::MyEndPaint(HWND hWnd, CONST PAINTSTRUCT *lpPaint)
	{
		auto& instance = HookDesktop::GetInstance();
		if (hWnd == g_desktopInfo.m_fileListWnd)
			return instance.OnEndPaint(hWnd, lpPaint);
		return instance.m_endPaintHook.m_oldEntry(hWnd, lpPaint);
	}

	// 动态文件列表窗口过程
	LRESULT HookDesktop::OnFileListWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_SIZE:
			g_fileListWndSizeEvent(LOWORD(lParam), HIWORD(lParam));
			break;
		}

		return CallWindowProc(m_oldFileListWndProc, hwnd, message, wParam, lParam);
	}

	// 动态父窗口过程
	LRESULT HookDesktop::OnParentWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_ERASEBKGND:
			if (!g_onDrawBackgroundEvent((HDC)wParam))
				return 1;
			break;
		}

		return CallWindowProc(m_oldParentWndProc, hwnd, message, wParam, lParam);
	}

	// 动态BeginPaint的hook
	HDC HookDesktop::OnBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint)
	{
		HDC res = m_beginPaintHook.m_oldEntry(hWnd, lpPaint);
		g_fileListBeginPaintEvent(lpPaint, res);
		return res;
	}

	// 动态EndPaint的hook
	BOOL HookDesktop::OnEndPaint(HWND hWnd, CONST PAINTSTRUCT *lpPaint)
	{
		g_fileListEndPaintEvent(lpPaint);
		return m_endPaintHook.m_oldEntry(hWnd, lpPaint);
	}
}
