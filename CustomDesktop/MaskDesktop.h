#pragma once
#include "BufferedDesktop.h"
#include "MDC.h"


class CMaskDesktop : public CBufferedDesktop
{
public:
	virtual ~CMaskDesktop();

	virtual BOOL Init();

protected:
	virtual BOOL OnEndPaint(HWND hWnd, CONST PAINTSTRUCT *lpPaint);
	virtual LRESULT OnFileListWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


	int m_size = 100;
	MDC m_mdc;
	POINTS m_curPos;
	SIZE m_scrSize;
};
