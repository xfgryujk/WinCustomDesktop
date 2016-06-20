#include "stdafx.h"
#include "MaskDesktop.h"
#include <comdef.h>
#include <gdiplus.h>
using namespace Gdiplus;


CMaskDesktop::~CMaskDesktop()
{
	Uninit(); // 为什么在父类析构函数不会调用子类的Uninit？
}

BOOL CMaskDesktop::Init()
{
	BOOL res = CCustomDesktop::Init();
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
		thiz->m_curPos = {};
		thiz->m_scrSize.cx = GetSystemMetrics(SM_CXSCREEN);
		thiz->m_scrSize.cy = GetSystemMetrics(SM_CYSCREEN);

		// 载入位图
		ULONG_PTR gdiplusToken = 0;
		GdiplusStartupInput gdiplusStartupInput;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		Image img(imgPath);
		thiz->m_mdc.Create(thiz->m_size, thiz->m_size);
		Graphics graphics(thiz->m_mdc);
		graphics.DrawImage(&img, 0, 0, thiz->m_size, thiz->m_size);
		GdiplusShutdown(gdiplusToken);

		// 子类化
		thiz->m_oldWndProc = (WNDPROC)SetWindowLongPtr(thiz->m_fileListWnd, GWLP_WNDPROC, (ULONG_PTR)WndProc);
		if (thiz->m_oldWndProc == NULL)
			thiz->Uninit();

		InvalidateRect(thiz->m_fileListWnd, NULL, TRUE);
	});

	return res;
}

void CMaskDesktop::Uninit()
{
	if (IsWindow(m_fileListWnd) && m_oldWndProc != NULL)
		SetWindowLongPtr(m_fileListWnd, GWLP_WNDPROC, (ULONG_PTR)m_oldWndProc);

	CCustomDesktop::Uninit();
}

BOOL CMaskDesktop::OnEndPaint(HWND hWnd, CONST PAINTSTRUCT *lpPaint)
{
	if ((HDC)m_mdc == NULL)
		return CCustomDesktop::OnEndPaint(hWnd, lpPaint);

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = 1;
	GdiAlphaBlend(lpPaint->hdc, m_curPos.x - m_size / 2, m_curPos.y - m_size / 2, m_size, m_size
		, m_mdc, 0, 0, m_size, m_size, bf);

	ULONG_PTR gdiplusToken = 0;
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	{
		Graphics graphics(lpPaint->hdc);
		SolidBrush brush(0xFE000000); // Alpha = 254 会改变目标的alpha值
		graphics.FillRectangle(&brush, 0, 0, m_curPos.x - m_size / 2 + 1, m_scrSize.cy + 1);
		graphics.FillRectangle(&brush, m_curPos.x - m_size / 2, 0, m_size, m_curPos.y - m_size / 2 + 1);
		graphics.FillRectangle(&brush, m_curPos.x + m_size / 2 - 1, 0, m_scrSize.cx - m_curPos.x - m_size / 2 + 2, m_scrSize.cy);
		graphics.FillRectangle(&brush, m_curPos.x - m_size / 2, m_curPos.y + m_size / 2 - 1, m_size, m_scrSize.cy - m_curPos.y - m_size / 2 + 2);
	}
	GdiplusShutdown(gdiplusToken);

	return CCustomDesktop::OnEndPaint(hWnd, lpPaint);
}

LRESULT CALLBACK CMaskDesktop::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (s_instance == NULL)
		return 0;
	CMaskDesktop* thiz = (CMaskDesktop*)s_instance;

	switch (message)
	{
	case WM_DISPLAYCHANGE:
		thiz->m_scrSize.cx = GetSystemMetrics(SM_CXSCREEN);
		thiz->m_scrSize.cy = GetSystemMetrics(SM_CYSCREEN);
		break;

	case WM_MOUSEMOVE:
	{
		POINTS lastPos = thiz->m_curPos;
		thiz->m_curPos = MAKEPOINTS(lParam);

		RECT rect;
		if (thiz->m_curPos.x < lastPos.x)
		{
			rect.left = thiz->m_curPos.x - thiz->m_size / 2;
			rect.right = lastPos.x + thiz->m_size / 2;
		}
		else
		{
			rect.left = lastPos.x - thiz->m_size / 2;
			rect.right = thiz->m_curPos.x + thiz->m_size / 2;
		}
		if (thiz->m_curPos.y < lastPos.y)
		{
			rect.top = thiz->m_curPos.y - thiz->m_size / 2;
			rect.bottom = lastPos.y + thiz->m_size / 2;
		}
		else
		{
			rect.top = lastPos.y - thiz->m_size / 2;
			rect.bottom = thiz->m_curPos.y + thiz->m_size / 2;
		}
		InvalidateRect(thiz->m_fileListWnd, &rect, FALSE);

		break;
	}
	}

	return CallWindowProc(thiz->m_oldWndProc, hwnd, message, wParam, lParam);
}
