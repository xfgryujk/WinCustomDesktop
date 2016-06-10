#pragma once
#include <map>


class CCustomDesktop
{
protected:
	// m_parentWnd -> CCustomDesktop*
	static std::map<HWND, CCustomDesktop*> s_instances;

	HWND m_desktopWnd = NULL;
	HWND m_parentWnd = NULL;
	HWND m_fileListWnd = NULL;

	WNDPROC m_oldWndProc = NULL;
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual void OnDrawBackground(HDC hdc);

public:
	virtual ~CCustomDesktop();

	virtual BOOL Init();
	virtual void Uninit();
};
