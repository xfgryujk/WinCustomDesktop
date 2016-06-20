#pragma once
#include "CustomDesktop.h"
#include "MDC.h"


class CMaskDesktop : public CCustomDesktop
{
public:
	virtual ~CMaskDesktop();

	virtual BOOL Init();
	virtual void Uninit();

protected:
	virtual BOOL OnEndPaint(HWND hWnd, CONST PAINTSTRUCT *lpPaint);


	int m_size = 100;
	MDC m_mdc;
	POINTS m_curPos;
	SIZE m_scrSize;

private:
	WNDPROC m_oldWndProc = NULL;
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};
