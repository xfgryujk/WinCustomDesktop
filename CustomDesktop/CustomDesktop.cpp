#include "stdafx.h"
#include "CustomDesktop.h"


// m_parentWnd -> CCustomDesktop*
std::map<HWND, CCustomDesktop*> CCustomDesktop::s_instances;


CCustomDesktop::~CCustomDesktop()
{
	Uninit();
}

// 初始化，寻找窗口句柄、子类化窗口
BOOL CCustomDesktop::Init()
{
	if (GetModuleHandle(_T("explorer.exe")) == NULL)
		return FALSE;

	m_desktopWnd = FindWindow(_T("Progman"), _T("Program Manager"));
	if (m_desktopWnd == NULL)
		return FALSE;
	m_parentWnd = FindWindowEx(m_desktopWnd, NULL, _T("SHELLDLL_DefView"), _T(""));
	if (m_parentWnd == NULL)
		return FALSE;
	m_fileListWnd = FindWindowEx(m_parentWnd, NULL, _T("SysListView32"), _T("FolderView"));
	if (m_fileListWnd == NULL)
		return FALSE;

	s_instances[m_parentWnd] = this;

	m_oldWndProc = (WNDPROC)SetWindowLongPtr(m_parentWnd, GWLP_WNDPROC, (ULONG_PTR)WndProc);
	if (m_oldWndProc == NULL)
	{
		s_instances.erase(m_parentWnd);
		return FALSE;
	}

	return TRUE;
}

// 释放
void CCustomDesktop::Uninit()
{
	if (IsWindow(m_parentWnd))
		SetWindowLongPtr(m_parentWnd, GWLP_WNDPROC, (ULONG_PTR)m_oldWndProc);

	s_instances.erase(m_parentWnd);
	m_oldWndProc = NULL;
	m_desktopWnd = m_parentWnd = m_fileListWnd = NULL;
}

// 静态窗口过程，传递WM_ERASEBKGND给动态的OnDrawBackground方法
LRESULT CALLBACK CCustomDesktop::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto it = s_instances.find(hwnd);
	if (it == s_instances.end())
		return 0;
	CCustomDesktop* thiz = it->second;

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
