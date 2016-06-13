#pragma once


class CCustomDesktop
{
protected:
	// 其实应该设计为单例的，但是这个类应该可以被继承...
	static CCustomDesktop* s_instance;

	HWND m_parentWnd = NULL;
	HWND m_fileListWnd = NULL;

	WNDPROC m_oldWndProc = NULL;
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual void OnDrawBackground(HDC hdc);

public:
	virtual ~CCustomDesktop();

	virtual BOOL Init();
	virtual BOOL Init(HWND fileListWnd);
	virtual void Uninit();
};
