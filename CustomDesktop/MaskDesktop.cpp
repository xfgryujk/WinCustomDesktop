#include "stdafx.h"
#include "MaskDesktop.h"
#include <comdef.h>
#include <gdiplus.h>
using namespace Gdiplus;


CMaskDesktop::~CMaskDesktop()
{
	Uninit();
}

BOOL CMaskDesktop::Init()
{
	BOOL res = CBufferedDesktop::Init();
	if (!res)
		return res;

	// 不能在DLLMain初始化GDI+
	SetTimer(m_parentWnd, 0, 100, [](HWND hwnd, UINT, UINT_PTR, DWORD){
		KillTimer(hwnd, 0);
		if (s_instance == NULL)
			return;
		CMaskDesktop* thiz = (CMaskDesktop*)s_instance;

		// 载入配置
		static const TCHAR configPath[] = _T("\\CustomDesktop.ini");
		static const TCHAR appName[] = _T("MaskDesktop");

		WCHAR imgPath[MAX_PATH];
		GetPrivateProfileStringW(appName, L"ImagePath", L"", imgPath, _countof(imgPath), configPath);
		thiz->m_size = GetPrivateProfileInt(appName, L"Size", 100, configPath);
		POINT pos;
		if (!GetCursorPos(&pos))
			pos = {};
		thiz->m_curPos = { (short)pos.x, (short)pos.y };
		RECT rect;
		if (!GetClientRect(thiz->m_fileListWnd, &rect))
			rect = {};
		thiz->m_scrSize = { rect.right - rect.left, rect.bottom - rect.top };

		// 载入位图
		ULONG_PTR gdiplusToken = 0;
		GdiplusStartupInput gdiplusStartupInput;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		Image img(imgPath);
		thiz->m_mdc.Create(thiz->m_size, thiz->m_size);
		Graphics graphics(thiz->m_mdc);
		graphics.DrawImage(&img, -5, -5, thiz->m_size + 10, thiz->m_size + 10);
		GdiplusShutdown(gdiplusToken);

		InvalidateRect(thiz->m_fileListWnd, NULL, TRUE);
	});

	return TRUE;
}

BOOL CMaskDesktop::OnEndPaint(HWND hWnd, CONST PAINTSTRUCT *lpPaint)
{
	if ((HDC)m_mdc == NULL)
		return CBufferedDesktop::OnEndPaint(hWnd, lpPaint);

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;
	GdiAlphaBlend(lpPaint->hdc, m_curPos.x - m_size / 2, m_curPos.y - m_size / 2, m_size, m_size
		, m_mdc, 0, 0, m_size, m_size, bf);

	HBRUSH brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
	RECT rect;
	rect = { 0, 0, m_curPos.x - m_size / 2 + 1, m_scrSize.cy };
	FillRect(lpPaint->hdc, &rect, brush);
	rect = { m_curPos.x - m_size / 2 + 1, 0, m_curPos.x + m_size / 2 - 1, m_curPos.y - m_size / 2 + 1 };
	FillRect(lpPaint->hdc, &rect, brush);
	rect = { m_curPos.x + m_size / 2 - 1, 0, m_scrSize.cx, m_scrSize.cy };
	FillRect(lpPaint->hdc, &rect, brush);
	rect = { m_curPos.x - m_size / 2 + 1, m_curPos.y + m_size / 2 - 1, m_curPos.x + m_size / 2 - 1, m_scrSize.cy };
	FillRect(lpPaint->hdc, &rect, brush);

	return CBufferedDesktop::OnEndPaint(hWnd, lpPaint);
}

LRESULT CMaskDesktop::OnFileListWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		m_scrSize = { LOWORD(lParam), HIWORD(lParam) };
		break;

	case WM_MOUSEMOVE:
	{
		POINTS lastPos = m_curPos;
		m_curPos = MAKEPOINTS(lParam);

		RECT rect;
		if (m_curPos.x < lastPos.x)
		{
			rect.left = m_curPos.x - m_size / 2 - 1;
			rect.right = lastPos.x + m_size / 2 + 1;
		}
		else
		{
			rect.left = lastPos.x - m_size / 2 - 1;
			rect.right = m_curPos.x + m_size / 2 + 1;
		}
		if (m_curPos.y < lastPos.y)
		{
			rect.top = m_curPos.y - m_size / 2 - 1;
			rect.bottom = lastPos.y + m_size / 2 + 1;
		}
		else
		{
			rect.top = lastPos.y - m_size / 2 - 1;
			rect.bottom = m_curPos.y + m_size / 2 + 1;
		}
		InvalidateRect(m_fileListWnd, &rect, TRUE);

		break;
	}
	}

	return CBufferedDesktop::OnFileListWndProc(hwnd, message, wParam, lParam);
}
