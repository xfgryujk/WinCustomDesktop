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
		g_global.m_oldTopWndProc = (WNDPROC)SetWindowLongPtr(g_global.m_topWnd, GWLP_WNDPROC, (ULONG_PTR)TopWndProc);
		if (g_global.m_oldTopWndProc == NULL) goto SubclassingTopWndFiled;

		// hook
		if (g_global.m_comctlModules.empty()) goto NoComctlModule;
		for (const auto& module : g_global.m_comctlModules)
		{
			IATHook<RedrawWindowType> redrawWindowHook(module, "user32.dll", "RedrawWindow", MyRedrawWindow);
			if (!redrawWindowHook.IsEnabled()) goto HookFailed;
			m_redrawWindowHooks.push_back(std::move(redrawWindowHook));
			IATHook<BeginPaintType> beginPaintHook(module, "user32.dll", "BeginPaint", MyBeginPaint);
			if (!beginPaintHook.IsEnabled()) goto HookFailed;
			m_beginPaintHooks.push_back(std::move(beginPaintHook));
			IATHook<EndPaintType> endPaintHook(module, "user32.dll", "EndPaint", MyEndPaint);
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
		SetWindowLongPtr(g_global.m_topWnd, GWLP_WNDPROC, (ULONG_PTR)g_global.m_oldTopWndProc);
	SubclassingTopWndFiled:
		SetWindowLongPtr(g_global.m_parentWnd, GWLP_WNDPROC, (ULONG_PTR)g_global.m_oldParentWndProc);
	SubclassingParentWndFiled:
		SetWindowLongPtr(g_global.m_fileListWnd, GWLP_WNDPROC, (ULONG_PTR)g_global.m_oldFileListWndProc);
		g_global.m_oldFileListWndProc = g_global.m_oldParentWndProc = g_global.m_oldTopWndProc = NULL;
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
		if (IsWindow(g_global.m_topWnd) && g_global.m_oldTopWndProc != NULL)
			SetWindowLongPtr(g_global.m_topWnd, GWLP_WNDPROC, (ULONG_PTR)g_global.m_oldTopWndProc);
		RedrawDesktop();

		// hook
		m_redrawWindowHooks.clear();
		m_beginPaintHooks.clear();
		m_endPaintHooks.clear();

		g_global.m_oldFileListWndProc = g_global.m_oldParentWndProc = g_global.m_oldTopWndProc = NULL;

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


	// 文件列表窗口过程
	LRESULT CALLBACK HookDesktop::FileListWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		LRESULT res = 0;
		if (!g_fileListWndProcEvent(message, wParam, lParam, res))
			return res;
		return CallWindowProc(g_global.m_oldFileListWndProc, hwnd, message, wParam, lParam);
	}

	// 父窗口过程
	LRESULT CALLBACK HookDesktop::ParentWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		LRESULT res = 0;
		if (!g_parentWndProcEvent(message, wParam, lParam, res))
			return res;
		return CallWindowProc(g_global.m_oldParentWndProc, hwnd, message, wParam, lParam);
	}

	// 顶级窗口过程
	LRESULT CALLBACK HookDesktop::TopWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		LRESULT res = 0;
		if (!g_topWndProcEvent(message, wParam, lParam, res))
			return res;
		return CallWindowProc(g_global.m_oldTopWndProc, hwnd, message, wParam, lParam);
	}


	// RedrawWindow的hook
	BOOL WINAPI HookDesktop::MyRedrawWindow(HWND hWnd, CONST RECT *lprcUpdate, HRGN hrgnUpdate, UINT flags)
	{
		if (hWnd == g_global.m_fileListWnd)
			g_fileListRedrawWindowEvent(lprcUpdate, hrgnUpdate, flags);
		return RedrawWindow(hWnd, lprcUpdate, hrgnUpdate, flags);
	}

	// BeginPaint的hook
	HDC WINAPI HookDesktop::MyBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint)
	{
		if (hWnd != g_global.m_fileListWnd)
			return BeginPaint(hWnd, lpPaint);

		g_global.m_isInBeginPaint = true;
		HDC res = BeginPaint(hWnd, lpPaint);
		g_global.m_isInBeginPaint = false;

		g_fileListBeginPaintEvent(lpPaint, res);
		return res;
	}

	// EndPaint的hook
	BOOL WINAPI HookDesktop::MyEndPaint(HWND hWnd, LPPAINTSTRUCT lpPaint)
	{
		if (hWnd == g_global.m_fileListWnd)
		{
			g_postDrawIconEvent(lpPaint->hdc);
			g_fileListEndPaintEvent(lpPaint);
		}
		return EndPaint(hWnd, lpPaint);
	}
}
