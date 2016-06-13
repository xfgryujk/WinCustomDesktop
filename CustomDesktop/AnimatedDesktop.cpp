#include "stdafx.h"
#include "AnimatedDesktop.h"
#include <comdef.h>
#include <gdiplus.h>
using namespace Gdiplus;


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
		TCHAR configPath[MAX_PATH];
		GetEnvironmentVariable(_T("windir"), configPath, _countof(configPath));
		_tcscat_s(configPath, _T("\\CustomDesktop.ini"));
		thiz->m_nImg = GetPrivateProfileInt(_T("image"), _T("ImageCount"), 0, configPath);
		if (thiz->m_nImg > 0)
		{
			WCHAR imgsPath[MAX_PATH];
			GetPrivateProfileStringW(L"image", L"ImagePath", L"", imgsPath, _countof(imgsPath), configPath);
			thiz->m_elapse = GetPrivateProfileInt(_T("image"), _T("Elapse"), 50, configPath);
			thiz->m_x = GetPrivateProfileInt(_T("image"), _T("X"), 0, configPath);
			thiz->m_y = GetPrivateProfileInt(_T("image"), _T("Y"), 0, configPath);
			thiz->m_width = GetPrivateProfileInt(_T("image"), _T("Width"), GetSystemMetrics(SM_CXSCREEN), configPath);
			thiz->m_height = GetPrivateProfileInt(_T("image"), _T("Height"), GetSystemMetrics(SM_CYSCREEN), configPath);

			// 载入位图
			ULONG_PTR gdiplusToken = 0;
			GdiplusStartupInput gdiplusStartupInput;
			GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
			thiz->m_mdc = new MDC*[thiz->m_nImg];
			for (int i = 0; i < thiz->m_nImg; i++)
			{
				WCHAR imgPath[MAX_PATH];
				swprintf_s(imgPath, imgsPath, i + 1);
				Image img(imgPath);
				thiz->m_mdc[i] = new MDC(thiz->m_width, thiz->m_height);
				Graphics graphics(thiz->m_mdc[i]->getDC());
				graphics.DrawImage(&img, 0, 0, thiz->m_width, thiz->m_height);
			}
			GdiplusShutdown(gdiplusToken);

			SetTimer(thiz->m_parentWnd, 0, thiz->m_elapse, [](HWND hwnd, UINT, UINT_PTR, DWORD){
				if (s_instance == NULL)
					return;
				CAnimatedDesktop* thiz = (CAnimatedDesktop*)s_instance;

				if (++thiz->m_curFrame >= thiz->m_nImg)
					thiz->m_curFrame = 0;
				RECT rect = { thiz->m_x, thiz->m_y, thiz->m_x + thiz->m_width, thiz->m_y + thiz->m_height };
				InvalidateRect(thiz->m_fileListWnd, &rect, FALSE);
				UpdateWindow(thiz->m_fileListWnd);
			});
		}
	});

	return res;
}

void CAnimatedDesktop::Uninit()
{
	KillTimer(m_parentWnd, 0);

	CCustomDesktop::Uninit();

	if (m_mdc != NULL)
	{
		for (int i = 0; i < m_nImg; i++)
			delete m_mdc[i];
		delete m_mdc;
		m_mdc = NULL;
	}
}

void CAnimatedDesktop::OnDrawBackground(HDC hdc)
{
	CCustomDesktop::OnDrawBackground(hdc);

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = 1;
	GdiAlphaBlend(hdc, m_x, m_y, m_width, m_height, m_mdc[m_curFrame]->getDC(), 0, 0, m_width, m_height, bf);
}
