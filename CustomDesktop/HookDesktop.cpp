#include "stdafx.h"
#include "HookDesktop.h"
#include "Global.h"
#include <CDEvents.h>
#include <CDAPI.h>


namespace cd
{
	// 初始化，子类化窗口，hook
	bool HookDesktop::Init()
	{
		if (m_hasInit) return true;
		if (GetModuleHandle(_T("explorer.exe")) == NULL) return false;

		// 子类化
		g_global.m_oldFileListWndProc = (WNDPROC)SetWindowLongPtr(g_global.m_fileListWnd, GWLP_WNDPROC, (ULONG_PTR)FileListWndProc);
		if (g_global.m_oldFileListWndProc == NULL) goto SubclassingFileListWndFiled;
		g_global.m_oldParentWndProc = (WNDPROC)SetWindowLongPtr(g_global.m_parentWnd, GWLP_WNDPROC, (ULONG_PTR)ParentWndProc);
		if (g_global.m_oldParentWndProc == NULL) goto SubclassingParentWndFiled;

		// hook
		if (g_global.m_comctlModules.empty()) goto NoComctlModule;
		for (const auto& module : g_global.m_comctlModules)
		{
			CIATHook<RedrawWindowType> redrawWindowHook(module, "user32.dll", "RedrawWindow", MyRedrawWindow);
			if (!redrawWindowHook.IsEnabled()) goto HookFailed;
			m_redrawWindowHooks.push_back(std::move(redrawWindowHook));
			CIATHook<BeginPaintType> beginPaintHook(module, "user32.dll", "BeginPaint", MyBeginPaint);
			if (!beginPaintHook.IsEnabled()) goto HookFailed;
			m_beginPaintHooks.push_back(std::move(beginPaintHook));
			CIATHook<EndPaintType> endPaintHook(module, "user32.dll", "EndPaint", MyEndPaint);
			if (!endPaintHook.IsEnabled()) goto HookFailed;
			m_endPaintHooks.push_back(std::move(endPaintHook));
		}

		m_hasInit = true;
		return true;

	HookFailed:
		m_redrawWindowHooks.clear();
		m_beginPaintHooks.clear();
		m_endPaintHooks.clear();
	NoComctlModule:
		SetWindowLongPtr(g_global.m_parentWnd, GWLP_WNDPROC, (ULONG_PTR)g_global.m_oldParentWndProc);
		g_global.m_oldParentWndProc = NULL;
	SubclassingParentWndFiled:
		SetWindowLongPtr(g_global.m_fileListWnd, GWLP_WNDPROC, (ULONG_PTR)g_global.m_oldFileListWndProc);
		g_global.m_oldFileListWndProc = NULL;
	SubclassingFileListWndFiled:
		return false;
	}

	// 去子类化、hook
	bool HookDesktop::Uninit()
	{
		if (!m_hasInit)
			return true;
		m_hasInit = false;

		// 子类化
		if (IsWindow(g_global.m_fileListWnd) && g_global.m_oldFileListWndProc != NULL)
			SetWindowLongPtr(g_global.m_fileListWnd, GWLP_WNDPROC, (ULONG_PTR)g_global.m_oldFileListWndProc);
		if (IsWindow(g_global.m_parentWnd) && g_global.m_oldParentWndProc != NULL)
			SetWindowLongPtr(g_global.m_parentWnd, GWLP_WNDPROC, (ULONG_PTR)g_global.m_oldParentWndProc);
		RedrawDesktop();

		// hook
		m_redrawWindowHooks.clear();
		m_beginPaintHooks.clear();
		m_endPaintHooks.clear();

		g_global.m_oldFileListWndProc = g_global.m_oldParentWndProc = NULL;

		return true;
	}

	HookDesktop::HookDesktop()
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

	// 静态RedrawWindow的hook，传递给动态的OnRedrawWindow方法
	BOOL WINAPI HookDesktop::MyRedrawWindow(HWND hWnd, CONST RECT *lprcUpdate, HRGN hrgnUpdate, UINT flags)
	{
		if (hWnd == g_global.m_fileListWnd)
			return HookDesktop::GetInstance().OnRedrawWindow(hWnd, lprcUpdate, hrgnUpdate, flags);
		return RedrawWindow(hWnd, lprcUpdate, hrgnUpdate, flags);
	}

	// 静态BeginPaint的hook，传递给动态的OnBeginPaint方法
	HDC WINAPI HookDesktop::MyBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint)
	{
		if (hWnd == g_global.m_fileListWnd)
			return HookDesktop::GetInstance().OnBeginPaint(hWnd, lpPaint);
		return BeginPaint(hWnd, lpPaint);
	}

	// 静态EndPaint的hook，传递给动态的OnEndPaint方法
	BOOL WINAPI HookDesktop::MyEndPaint(HWND hWnd, LPPAINTSTRUCT lpPaint)
	{
		if (hWnd == g_global.m_fileListWnd)
			return HookDesktop::GetInstance().OnEndPaint(hWnd, lpPaint);
		return EndPaint(hWnd, lpPaint);
	}

	// 动态文件列表窗口过程
	LRESULT HookDesktop::OnFileListWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (!g_fileListWndProcEvent(message, wParam, lParam))
			return 1;
		return CallWindowProc(g_global.m_oldFileListWndProc, hwnd, message, wParam, lParam);
	}

	// 动态父窗口过程
	LRESULT HookDesktop::OnParentWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (!g_parentWndProcEvent(message, wParam, lParam))
			return 1;
		return CallWindowProc(g_global.m_oldParentWndProc, hwnd, message, wParam, lParam);
	}

	// 动态RedrawWindow的hook
	BOOL HookDesktop::OnRedrawWindow(HWND hWnd, CONST RECT *lprcUpdate, HRGN hrgnUpdate, UINT flags)
	{
		g_fileListRedrawWindowEvent(lprcUpdate, hrgnUpdate, flags);
		return RedrawWindow(hWnd, lprcUpdate, hrgnUpdate, flags);
	}

	// 动态BeginPaint的hook
	HDC HookDesktop::OnBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint)
	{
		g_global.m_isInBeginPaint = true;
		HDC res = BeginPaint(hWnd, lpPaint);
		g_global.m_isInBeginPaint = false;

		g_fileListBeginPaintEvent(lpPaint, res);
		return res;
	}

	// 动态EndPaint的hook
	BOOL HookDesktop::OnEndPaint(HWND hWnd, LPPAINTSTRUCT lpPaint)
	{
		g_postDrawIconEvent(lpPaint->hdc);
		g_fileListEndPaintEvent(lpPaint);
		return EndPaint(hWnd, lpPaint);
	}
}
