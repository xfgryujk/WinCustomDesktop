#include "stdafx.h"
#include "AnimatedDesktop.h"
#include <comdef.h>
#include <gdiplus.h>
using namespace Gdiplus;


CAnimatedDesktop::~CAnimatedDesktop()
{
	Uninit();
}

BOOL CAnimatedDesktop::Init()
{
	BOOL res = CCustomDesktop::Init();
	if (!res)
		return res;

	// 不能在DLLMain初始化GDI+
	SetTimer(m_parentWnd, 0, 100, [](HWND hwnd, UINT, UINT_PTR, DWORD){
		KillTimer(hwnd, 0);
		if (s_instance == NULL)
			return;
		CAnimatedDesktop* thiz = (CAnimatedDesktop*)s_instance;

		// 载入配置
		static const TCHAR configPath[] = _T("\\CustomDesktop.ini");
		static const TCHAR appName[] = _T("AnimatedDesktop");

		int nImg = GetPrivateProfileInt(appName, _T("ImageCount"), 0, configPath);
		if (nImg > 0)
		{
			WCHAR imgsPath[MAX_PATH];
			GetPrivateProfileStringW(appName, L"ImagePath", L"", imgsPath, _countof(imgsPath), configPath);
			thiz->m_elapse = GetPrivateProfileInt(appName, _T("Elapse"), 50, configPath);
			thiz->m_pos.x = GetPrivateProfileInt(appName, _T("X"), 0, configPath);
			thiz->m_pos.y = GetPrivateProfileInt(appName, _T("Y"), 0, configPath);
			thiz->m_size.cx = GetPrivateProfileInt(appName, _T("Width"), GetSystemMetrics(SM_CXSCREEN), configPath);
			thiz->m_size.cy = GetPrivateProfileInt(appName, _T("Height"), GetSystemMetrics(SM_CYSCREEN), configPath);

			// 载入位图
			ULONG_PTR gdiplusToken = 0;
			GdiplusStartupInput gdiplusStartupInput;
			GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
			thiz->m_mdc.resize(nImg);
			for (int i = 0; i < nImg; i++)
			{
				WCHAR imgPath[MAX_PATH];
				swprintf_s(imgPath, imgsPath, i + 1);
				Image img(imgPath);
				thiz->m_mdc[i].Create(thiz->m_size.cx, thiz->m_size.cy);
				Graphics graphics(thiz->m_mdc[i]);
				graphics.DrawImage(&img, 0, 0, thiz->m_size.cx, thiz->m_size.cy);
			}
			GdiplusShutdown(gdiplusToken);
			thiz->m_nImg = nImg;

			// 自动重画
			if (nImg > 1)
				SetTimer(thiz->m_parentWnd, 0, thiz->m_elapse, TimerProc);
		}
	});

	return TRUE;
}

void CAnimatedDesktop::Uninit()
{
	KillTimer(m_parentWnd, 0);

	CCustomDesktop::Uninit();

	m_mdc.clear();
}

VOID CAnimatedDesktop::TimerProc(HWND, UINT, UINT_PTR, DWORD)
{
	if (s_instance == NULL)
		return;
	CAnimatedDesktop* thiz = (CAnimatedDesktop*)s_instance;

	if (++thiz->m_curFrame >= thiz->m_nImg)
		thiz->m_curFrame = 0;
	RECT rect = { thiz->m_pos.x, thiz->m_pos.y, thiz->m_pos.x + thiz->m_size.cx, thiz->m_pos.y + thiz->m_size.cy };
	InvalidateRect(thiz->m_fileListWnd, &rect, TRUE);
}

void CAnimatedDesktop::OnDrawBackground(HDC hdc)
{
	CCustomDesktop::OnDrawBackground(hdc);
	if (m_nImg <= 0)
		return;

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;
	GdiAlphaBlend(hdc, m_pos.x, m_pos.y, m_size.cx, m_size.cy, m_mdc[m_curFrame], 0, 0, m_size.cx, m_size.cy, bf);
}
