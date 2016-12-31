// MDC.cpp : 缓存设备场景类。
//

#include "stdafx.h"
#include <MDC.h>


namespace cd
{
	MDC::MDC(int width, int height)
	{
		Create(width, height);
	}

	MDC::~MDC()
	{
		Release();
	}

	bool MDC::Create(int width, int height, WORD biBitCount)
	{
		if (m_mdc != NULL)
			Release();

		m_mdc = CreateCompatibleDC(NULL);
		if (m_mdc == NULL)
			return false;

		m_width = width;
		m_height = height;
		m_BPP = biBitCount / 8;

		BITMAPINFO bmpInfo;
		ZeroMemory(&bmpInfo.bmiHeader, sizeof(BITMAPINFOHEADER));
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biWidth = width;
		bmpInfo.bmiHeader.biHeight = height;
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biBitCount = biBitCount;
		bmpInfo.bmiHeader.biCompression = BI_RGB;
		bmpInfo.bmiHeader.biSizeImage = width * height * m_BPP;

		HBITMAP bmp = CreateDIBSection(NULL, &bmpInfo, DIB_RGB_COLORS, &m_pBits, NULL, 0);
		if (bmp == NULL)
		{
			DeleteDC(m_mdc);
			m_mdc = NULL;
			return false;
		}

		m_oldBmp = (HBITMAP)SelectObject(m_mdc, bmp);
		return true;
	}

	void MDC::Release()
	{
		if (m_mdc != NULL)
		{
			DeleteObject(SelectObject(m_mdc, m_oldBmp));
			DeleteDC(m_mdc);
			m_width = m_height = m_BPP = 0;
			m_pBits = NULL;
		}
	}

	MDC::operator HDC ()
	{
		return m_mdc;
	}

	void* MDC::GetPixelAddress(UINT x, UINT y)
	{
		if (m_pBits == NULL || x < 0 || x >= m_width || y < 0 || y >= m_height)
			return NULL;
		return (void*)((uintptr_t)m_pBits + ((m_height - y - 1) * m_width + x) * m_BPP);
	}
}
