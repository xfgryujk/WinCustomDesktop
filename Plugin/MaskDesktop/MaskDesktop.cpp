#include "stdafx.h"
#include "MaskDesktop.h"
#include <CDEvents.h>
#include <CDAPI.h>
#include "MDConfig.h"


MaskDesktop::MaskDesktop(HMODULE hModule) : 
	m_module(hModule)
{
	// 载入图片
	CImage img;
	img.Load(g_config.m_imagePath.c_str());
	m_img.Create(g_config.m_size, g_config.m_size, 32, CImage::createAlphaChannel);
	img.Draw(m_img.GetDC(), -5, -5, g_config.m_size + 10, g_config.m_size + 10);
	m_img.ReleaseDC();

	// 监听事件
	cd::g_fileListWndProcEvent.AddListener(std::bind(&MaskDesktop::OnFileListWndProc, this, std::placeholders::_1, 
		std::placeholders::_2, std::placeholders::_3), m_module);
	cd::g_postDrawIconEvent.AddListener(std::bind(&MaskDesktop::OnFileListEndPaint, this, std::placeholders::_1), m_module);

	cd::RedrawDesktop();
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

		cd::RedrawDesktop(&rect);
	}
	return true;
}

bool MaskDesktop::OnFileListEndPaint(HDC& hdc)
{
	if (m_img.IsNull())
		return true;

	m_img.AlphaBlend(hdc, m_curPos.x - g_config.m_size / 2, m_curPos.y - g_config.m_size / 2);

	HBRUSH brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
	SIZE scrSize;
	cd::GetDesktopSize(scrSize);
	RECT rect;
	rect = { 0, 0, m_curPos.x - g_config.m_size / 2 + 1, scrSize.cy };
	FillRect(hdc, &rect, brush);
	rect = { m_curPos.x - g_config.m_size / 2 + 1, 0, m_curPos.x + g_config.m_size / 2 - 1, m_curPos.y - g_config.m_size / 2 + 1 };
	FillRect(hdc, &rect, brush);
	rect = { m_curPos.x + g_config.m_size / 2 - 1, 0, scrSize.cx, scrSize.cy };
	FillRect(hdc, &rect, brush);
	rect = { m_curPos.x - g_config.m_size / 2 + 1, m_curPos.y + g_config.m_size / 2 - 1, m_curPos.x + g_config.m_size / 2 - 1, scrSize.cy };
	FillRect(hdc, &rect, brush);

	return true;
}	
