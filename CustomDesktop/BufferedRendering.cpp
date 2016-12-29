#include "stdafx.h"
#include "BufferedRendering.h"
#include "DesktopInfo.h"
#include "InternalEvents.h"


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
		if (!m_bufferDC.Create(g_desktopInfo.m_wndSize.cx, g_desktopInfo.m_wndSize.cy))
			return false;

		// 监听事件
		g_fileListWndSizeEvent.AddListener(std::bind(&BufferedRendering::OnFileListWndSize, this, std::placeholders::_1, std::placeholders::_2));
		g_fileListBeginPaintEvent.AddListener(std::bind(&BufferedRendering::OnFileListBeginPaint, this, std::placeholders::_1, std::placeholders::_2));
		g_fileListEndPaintEvent.AddListener(std::bind(&BufferedRendering::OnFileListEndPaint, this, std::placeholders::_1));

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


	bool BufferedRendering::OnFileListWndSize(int width, int height)
	{
		m_bufferDC.Create(width, height);
		return true;
	}

	bool BufferedRendering::OnFileListBeginPaint(LPPAINTSTRUCT lpPaint, HDC& res)
	{
		if (res != NULL)
		{
			m_originalDC = res;
			res = lpPaint->hdc = m_bufferDC;
		}
		return true;
	}

	bool BufferedRendering::OnFileListEndPaint(CONST PAINTSTRUCT *lpPaint)
	{
		if (lpPaint->hdc != NULL)
		{
			const_cast<PAINTSTRUCT*>(lpPaint)->hdc = m_originalDC;

			BitBlt(lpPaint->hdc, lpPaint->rcPaint.left, lpPaint->rcPaint.top, lpPaint->rcPaint.right - lpPaint->rcPaint.left,
				lpPaint->rcPaint.bottom - lpPaint->rcPaint.top, m_bufferDC, lpPaint->rcPaint.left, lpPaint->rcPaint.top, SRCCOPY);
		}
		return true;
	}
}
