#pragma once
#include "CustomDesktop.h"
#include "MDC.h"
#include <vector>


class CAnimatedDesktop : public CCustomDesktop
{
public:
	virtual ~CAnimatedDesktop();

	virtual BOOL Init();
	virtual void Uninit();

protected:
	static VOID CALLBACK TimerProc(HWND, UINT, UINT_PTR, DWORD);

	virtual void OnDrawBackground(HDC hdc);


	int m_nImg = 0;
	int m_elapse = 50;
	POINT m_pos;
	SIZE m_size;
	std::vector<MDC> m_mdc;
	int m_curFrame = 0;
};
