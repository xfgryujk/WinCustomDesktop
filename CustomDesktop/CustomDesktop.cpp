#include "stdafx.h"
#include "CustomDesktop.h"


// 其实应该设计为单例的，但是这个类应该可以被继承...
CCustomDesktop* CCustomDesktop::s_instance = NULL;


CCustomDesktop::~CCustomDesktop()
{
	Uninit();
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

// 初始化，子类化窗口
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

	m_oldWndProc = (WNDPROC)SetWindowLongPtr(m_parentWnd, GWLP_WNDPROC, (ULONG_PTR)WndProc);
	if (m_oldWndProc == NULL)
	{
		s_instance = NULL;
		return FALSE;
	}

	return TRUE;
}

// 释放，貌似在不同线程，运气不好可能会崩溃...懒得加锁了
void CCustomDesktop::Uninit()
{
	if (IsWindow(m_parentWnd))
	{
		SetWindowLongPtr(m_parentWnd, GWLP_WNDPROC, (ULONG_PTR)m_oldWndProc);
		if (IsWindow(m_fileListWnd))
		{
			InvalidateRect(m_fileListWnd, NULL, TRUE);
			UpdateWindow(m_fileListWnd);
		}
	}

	s_instance = NULL;
	m_oldWndProc = NULL;
	m_topWnd = m_parentWnd = m_fileListWnd = NULL;
}

// 静态窗口过程，传递WM_ERASEBKGND给动态的OnDrawBackground方法
LRESULT CALLBACK CCustomDesktop::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (s_instance == NULL)
		return 0;
	CCustomDesktop* thiz = s_instance;

	if (msg == WM_ERASEBKGND)
	{
		thiz->OnDrawBackground((HDC)wParam);
		return 1;
	}

	return CallWindowProc(thiz->m_oldWndProc, hwnd, msg, wParam, lParam);
}

// 实现绘制背景
void CCustomDesktop::OnDrawBackground(HDC hdc)
{
	if (m_oldWndProc != NULL)
		CallWindowProc(m_oldWndProc, m_parentWnd, WM_ERASEBKGND, (WPARAM)hdc, NULL);
}
