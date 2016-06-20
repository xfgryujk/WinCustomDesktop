#pragma once
#include "CustomDesktop.h"
#include "MDC.h"


class CBufferedDesktop : public CCustomDesktop
{
public:
	virtual ~CBufferedDesktop();

	virtual BOOL Init();
	virtual void Uninit();

private:
	WNDPROC m_oldWndProc = NULL;
	HDC m_originalDC = NULL;
	static LRESULT CALLBACK FileListWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	virtual HDC OnBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint);
	virtual BOOL OnEndPaint(HWND hWnd, CONST PAINTSTRUCT *lpPaint);
	virtual LRESULT OnFileListWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


	MDC m_bufferDC;
};
