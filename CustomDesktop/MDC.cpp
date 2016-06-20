// MDC.cpp : 缓存设备场景类。
//

#include "stdafx.h"
#include "MDC.h"


MDC::MDC(int width, int height)
{
	Create(width, height);
}

MDC::~MDC()
{
	Release();
}

BOOL MDC::Create(int width, int height, WORD biBitCount)
{
	if (m_mdc != NULL)
		Release();

	m_mdc = CreateCompatibleDC(NULL);
	if (m_mdc == NULL)
		return FALSE;

	BITMAPINFO bmpInfo;
	ZeroMemory(&bmpInfo.bmiHeader, sizeof(BITMAPINFOHEADER));
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = width;
	bmpInfo.bmiHeader.biHeight = height;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = biBitCount;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage = width * height * bmpInfo.bmiHeader.biBitCount / 8;

	void* pBits;
	HBITMAP bmp = CreateDIBSection(NULL, &bmpInfo, DIB_RGB_COLORS, &pBits, NULL, 0);
	if (bmp == NULL)
	{
		DeleteDC(m_mdc);
		m_mdc = NULL;
		return FALSE;
	}

	m_oldBmp = (HBITMAP)SelectObject(m_mdc, bmp);
	return TRUE;
}

void MDC::Release()
{
	if (m_mdc != NULL)
	{
		DeleteObject(SelectObject(m_mdc, m_oldBmp));
		DeleteDC(m_mdc);
	}
}

MDC::operator HDC ()
{
	return m_mdc;
}
