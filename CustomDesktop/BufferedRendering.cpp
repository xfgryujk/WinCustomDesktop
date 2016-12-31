#include "stdafx.h"
#include "BufferedRendering.h"
#include "Global.h"
#include <CDEvents.h>


namespace cd
{
	BufferedRendering::BufferedRendering()
	{
		Init();
	}

	BufferedRendering::~BufferedRendering()
	{
		Uninit();
	}


	bool BufferedRendering::Init()
	{
		if (m_hasInit)
			return true;

		// 创建缓冲DC
		if (!m_bufferDC.Create(g_global.m_wndSize.cx, g_global.m_wndSize.cy, 24))
			return false;

		// 监听事件
		g_fileListWndSizeEvent.AddListener([this](int width, int height){ m_bufferDC.Create(width, height); return true; });
		g_fileListBeginPaintEvent.AddListener(std::bind(&BufferedRendering::OnFileListBeginPaint, this, std::placeholders::_1, std::placeholders::_2));
		g_fileListEndPaintEvent.AddListener(std::bind(&BufferedRendering::OnFileListEndPaint, this, std::placeholders::_1));
		// 防止XP下BeginPaint擦除背景造成闪烁
		// 但是替换掉dc后XP下WM_ERASEBKGND画不了背景？？
		// XP下画桌面用PaintDesktop，但是无法画到内存DC
		// http://www.progtown.com/topic247718-gdi-misunderstanding-of-operation-paintdesktop.html
		// > You cannot PaintDesktop into a memory DC because PaintDesktop does an EnumDisplayMonitors 
		// to render the desktop on each monitor, but a memory DC does not have any monitors.
		// 还是自己读取壁纸再画吧 HKCU/Control Panel/Desktop/Wallpaper
		g_drawBackgroundEvent.AddListener([this](HDC& hdc, bool isInBeginPaint){
			if (isInBeginPaint)
			{
				hdc = m_bufferDC;
				return false;
			}
			return true;
		});

		m_hasInit = true;
		return true;
	}

	bool BufferedRendering::Uninit()
	{
		if (!m_hasInit)
			return true;
		m_hasInit = false;
		return true;
	}

	bool BufferedRendering::OnFileListBeginPaint(LPPAINTSTRUCT lpPaint, HDC& res)
	{
		if (res != NULL)
		{
			m_originalDC = res;
			res = lpPaint->hdc = m_bufferDC;

			BitBlt(m_bufferDC, lpPaint->rcPaint.left, lpPaint->rcPaint.top, lpPaint->rcPaint.right - lpPaint->rcPaint.left,
				lpPaint->rcPaint.bottom - lpPaint->rcPaint.top, m_originalDC, lpPaint->rcPaint.left, lpPaint->rcPaint.top, SRCCOPY);
		}
		return true;
	}

	bool BufferedRendering::OnFileListEndPaint(LPPAINTSTRUCT lpPaint)
	{
		if (lpPaint->hdc != NULL)
		{
			BitBlt(m_originalDC, lpPaint->rcPaint.left, lpPaint->rcPaint.top, lpPaint->rcPaint.right - lpPaint->rcPaint.left,
				lpPaint->rcPaint.bottom - lpPaint->rcPaint.top, m_bufferDC, lpPaint->rcPaint.left, lpPaint->rcPaint.top, SRCCOPY);

			lpPaint->hdc = m_originalDC;
		}
		return true;
	}
}
