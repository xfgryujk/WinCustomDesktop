#pragma once
#include "CustomDesktop.h"
#include "MDC.h"


class CAnimatedDesktop : public CCustomDesktop
{
public:
	virtual BOOL Init();
	virtual void Uninit();

protected:
	static VOID CALLBACK TimerProc(HWND, UINT, UINT_PTR, DWORD);

	virtual void OnDrawBackground(HDC hdc);


	int m_nImg = 0;
	int m_elapse = 50;
	int m_x = 0, m_y = 0, m_width = 0, m_height = 0;
	MDC** m_mdc = NULL;
	int m_curFrame = 0;
};
