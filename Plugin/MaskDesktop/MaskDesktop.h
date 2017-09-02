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

	void OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& res, bool& pass);
	void OnPostDrawIcon(HDC& hdc);

	void OnAppendTrayMenu(HMENU menu);
	void OnChooseMenuItem(UINT menuID, bool& pass);
};
