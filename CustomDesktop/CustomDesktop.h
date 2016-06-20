#pragma once
#include "IATHook.h"


class CCustomDesktop
{
protected:
	// 其实应该设计为单例的，但是这个类应该可以被继承...
	static CCustomDesktop* s_instance;

	HWND m_topWnd = NULL;
	HWND m_parentWnd = NULL;
	HWND m_fileListWnd = NULL;

private:
	WNDPROC m_oldWndProc = NULL;
	static LRESULT CALLBACK ParentWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	typedef HDC (WINAPI* BeginPaintType)(HWND hWnd, LPPAINTSTRUCT lpPaint);
	CIATHook<BeginPaintType> m_beginPaintHook;
	static HDC WINAPI MyBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint);

	typedef BOOL (WINAPI* EndPaintType)(HWND hWnd, CONST PAINTSTRUCT *lpPaint);
	CIATHook<EndPaintType> m_endPaintHook;
	static BOOL WINAPI MyEndPaint(HWND hWnd, CONST PAINTSTRUCT *lpPaint);

protected:
	virtual LRESULT OnParentWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnDrawBackground(HDC hdc);
	virtual HDC OnBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint);
	virtual BOOL OnEndPaint(HWND hWnd, CONST PAINTSTRUCT *lpPaint);

public:
	CCustomDesktop();
	virtual ~CCustomDesktop();

	virtual BOOL Init();
	virtual void Uninit();
protected:
	virtual BOOL Init(HWND fileListWnd);
};
