#pragma once
#include <atlimage.h>


class MaskDesktop final
{
public:
	MaskDesktop(HMODULE hModule);

private:
	HMODULE m_module;

	CImage m_img;
	POINTS m_curPos;

	const UINT m_menuID;


	void InitImg();

	bool OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& res);
	bool OnPostDrawIcon(HDC& hdc);

	bool OnAppendTrayMenu(HMENU menu);
	bool OnChooseMenuItem(UINT menuID);
};
