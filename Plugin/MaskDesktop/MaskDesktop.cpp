#include "stdafx.h"
#include "MaskDesktop.h"
#include <comdef.h>
#include <gdiplus.h>
using namespace Gdiplus;
#include <CDEvents.h>
#include <CDAPI.h>
#include "MDConfig.h"


MaskDesktop::MaskDesktop(HMODULE hModule) : 
	m_module(hModule)
{
	cd::ExecInMainThread([this]{
		// 载入图片
		ULONG_PTR gdiplusToken = 0;
		GdiplusStartupInput gdiplusStartupInput;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		Image img(g_config.m_imagePath.c_str());
		m_mdc.Create(g_config.m_size, g_config.m_size);
		Graphics graphics(m_mdc);
		graphics.DrawImage(&img, -5, -5, g_config.m_size + 10, g_config.m_size + 10);
		GdiplusShutdown(gdiplusToken);

		cd::GetWndSize(m_scrSize);

		// 监听事件
		cd::g_fileListWndSizeEvent.AddListener([this](int width, int height){
			m_scrSize = { width, height };
			return true;
		}, m_module);
		cd::g_fileListWndProcEvent.AddListener(std::bind(&MaskDesktop::OnFileListWndProc, this, std::placeholders::_1, 
			std::placeholders::_2, std::placeholders::_3), m_module);
		cd::g_fileListEndPaintEvent.AddListener(std::bind(&MaskDesktop::OnFileListEndPaint, this, std::placeholders::_1), m_module);

		InvalidateRect(cd::GetFileListHwnd(), NULL, TRUE);
	});
}


bool MaskDesktop::OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_MOUSEMOVE)
	{
		POINTS lastPos = m_curPos;
		m_curPos = MAKEPOINTS(lParam);

		RECT rect;
		if (m_curPos.x < lastPos.x)
		{
			rect.left = m_curPos.x - g_config.m_size / 2 - 1;
			rect.right = lastPos.x + g_config.m_size / 2 + 1;
		}
		else
		{
			rect.left = lastPos.x - g_config.m_size / 2 - 1;
			rect.right = m_curPos.x + g_config.m_size / 2 + 1;
		}
		if (m_curPos.y < lastPos.y)
		{
			rect.top = m_curPos.y - g_config.m_size / 2 - 1;
			rect.bottom = lastPos.y + g_config.m_size / 2 + 1;
		}
		else
		{
			rect.top = lastPos.y - g_config.m_size / 2 - 1;
			rect.bottom = m_curPos.y + g_config.m_size / 2 + 1;
		}

		InvalidateRect(cd::GetFileListHwnd(), &rect, TRUE);
	}
	return true;
}

bool MaskDesktop::OnFileListEndPaint(LPPAINTSTRUCT lpPaint)
{
	if ((HDC)m_mdc == NULL)
		return true;

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;
	GdiAlphaBlend(lpPaint->hdc, m_curPos.x - g_config.m_size / 2, m_curPos.y - g_config.m_size / 2, g_config.m_size, g_config.m_size
		, m_mdc, 0, 0, g_config.m_size, g_config.m_size, bf);

	HBRUSH brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
	RECT rect;
	rect = { 0, 0, m_curPos.x - g_config.m_size / 2 + 1, m_scrSize.cy };
	FillRect(lpPaint->hdc, &rect, brush);
	rect = { m_curPos.x - g_config.m_size / 2 + 1, 0, m_curPos.x + g_config.m_size / 2 - 1, m_curPos.y - g_config.m_size / 2 + 1 };
	FillRect(lpPaint->hdc, &rect, brush);
	rect = { m_curPos.x + g_config.m_size / 2 - 1, 0, m_scrSize.cx, m_scrSize.cy };
	FillRect(lpPaint->hdc, &rect, brush);
	rect = { m_curPos.x - g_config.m_size / 2 + 1, m_curPos.y + g_config.m_size / 2 - 1, m_curPos.x + g_config.m_size / 2 - 1, m_scrSize.cy };
	FillRect(lpPaint->hdc, &rect, brush);

	return true;
}	
