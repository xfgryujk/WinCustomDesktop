#include "stdafx.h"
#include "CustomDesktop.h"


// 其实应该设计为单例的，但是这个类应该可以被继承...
CCustomDesktop* CCustomDesktop::s_instance = NULL;


CCustomDesktop::CCustomDesktop() :
	m_beginPaintHook(GetModuleHandle(_T("comctl32.dll")), "user32.dll", "BeginPaint"),
	m_endPaintHook(GetModuleHandle(_T("comctl32.dll")), "user32.dll", "EndPaint")
{

}

CCustomDesktop::~CCustomDesktop()
{
	Uninit(); // 这时候子类已经被析构，Uninit被当做实函数，所以不会调用子类的Uninit！！
}

// 初始化，寻找窗口句柄
BOOL CCustomDesktop::Init()
{
	HWND topWnd = FindWindow(_T("Progman"), _T("Program Manager"));
	if (topWnd == NULL)
		return FALSE;
	HWND parentWnd = FindWindowEx(topWnd, NULL, _T("SHELLDLL_DefView"), _T(""));
	if (parentWnd == NULL)
		return FALSE;
	HWND fileListWnd = FindWindowEx(parentWnd, NULL, _T("SysListView32"), _T("FolderView"));
	if (fileListWnd == NULL)
		return FALSE;

	return Init(fileListWnd);
}

// 初始化，子类化窗口，hook
BOOL CCustomDesktop::Init(HWND fileListWnd)
{
	if (fileListWnd == NULL)
		return FALSE;
	if (GetModuleHandle(_T("explorer.exe")) == NULL)
		return FALSE;

	if (s_instance != NULL)
		return FALSE;
	s_instance = this;

	m_fileListWnd = fileListWnd;
	m_parentWnd = GetParent(fileListWnd);
	m_topWnd = GetParent(m_parentWnd);

	m_oldWndProc = (WNDPROC)SetWindowLongPtr(m_parentWnd, GWLP_WNDPROC, (ULONG_PTR)ParentWndProc);
	if (m_oldWndProc == NULL)
	{
		s_instance = NULL;
		return FALSE;
	}

	m_beginPaintHook.Hook(MyBeginPaint);
	m_endPaintHook.Hook(MyEndPaint);

	return TRUE;
}

// 释放，貌似在不同线程，运气不好可能会崩溃...懒得加锁了
void CCustomDesktop::Uninit()
{
	if (IsWindow(m_parentWnd) && m_oldWndProc != NULL)
	{
		SetWindowLongPtr(m_parentWnd, GWLP_WNDPROC, (ULONG_PTR)m_oldWndProc);
		if (IsWindow(m_fileListWnd))
			InvalidateRect(m_fileListWnd, NULL, TRUE);
	}

	m_endPaintHook.Unhook();
	m_beginPaintHook.Unhook();

	s_instance = NULL;
	m_oldWndProc = NULL;
	m_topWnd = m_parentWnd = m_fileListWnd = NULL;
}

// 静态窗口过程，传递给动态窗口过程
LRESULT CALLBACK CCustomDesktop::ParentWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (s_instance == NULL)
		return 0;
	CCustomDesktop* thiz = s_instance;

	return thiz->OnParentWndProc(hwnd, message, wParam, lParam);
}

// 动态窗口过程，传递WM_ERASEBKGND给OnDrawBackground方法
LRESULT CCustomDesktop::OnParentWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_ERASEBKGND)
	{
		OnDrawBackground((HDC)wParam);
		return 1;
	}

	return CallWindowProc(m_oldWndProc, hwnd, message, wParam, lParam);
}

// 静态BeginPaint的hook，传递给动态的OnBeginPaint方法
HDC WINAPI CCustomDesktop::MyBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint)
{
	if (s_instance == NULL)
		return NULL;
	CCustomDesktop* thiz = s_instance;

	if (hWnd == thiz->m_fileListWnd)
		return thiz->OnBeginPaint(hWnd, lpPaint);

	return thiz->m_beginPaintHook.GetOriginalFunction()(hWnd, lpPaint);
}

// 静态EndPaint的hook，传递给动态的OnEndPaint方法
BOOL WINAPI CCustomDesktop::MyEndPaint(HWND hWnd, CONST PAINTSTRUCT *lpPaint)
{
	if (s_instance == NULL)
		return TRUE;
	CCustomDesktop* thiz = s_instance;

	if (hWnd == thiz->m_fileListWnd)
		return thiz->OnEndPaint(hWnd, lpPaint);

	return thiz->m_endPaintHook.GetOriginalFunction()(hWnd, lpPaint);
}

// 实现绘制背景
void CCustomDesktop::OnDrawBackground(HDC hdc)
{
	if (m_oldWndProc != NULL)
		CallWindowProc(m_oldWndProc, m_parentWnd, WM_ERASEBKGND, (WPARAM)hdc, NULL);
}

HDC CCustomDesktop::OnBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint)
{
	return m_beginPaintHook.GetOriginalFunction()(hWnd, lpPaint);
}

BOOL CCustomDesktop::OnEndPaint(HWND hWnd, CONST PAINTSTRUCT *lpPaint)
{
	return m_endPaintHook.GetOriginalFunction()(hWnd, lpPaint);
}
