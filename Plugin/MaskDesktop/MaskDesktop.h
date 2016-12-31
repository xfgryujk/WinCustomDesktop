#pragma once
#include <MDC.h>


class MaskDesktop final
{
public:
	MaskDesktop(HMODULE hModule);

private:
	HMODULE m_module;

	cd::MDC m_mdc;
	POINTS m_curPos;


	bool OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam);
	bool OnFileListEndPaint(LPPAINTSTRUCT lpPaint);
};
