// MDC.cpp : 缓存设备场景类。
//

#include "stdafx.h"
#include "MDC.h"


MDC::MDC()
{
	mdc    = NULL;
	oldBmp = NULL;
}

MDC::MDC(int width, int height)
{
	BITMAPINFO bmpInfo;
	ZeroMemory(&bmpInfo.bmiHeader, sizeof(BITMAPINFOHEADER));
	bmpInfo.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth       = width;
	bmpInfo.bmiHeader.biHeight      = height;
	bmpInfo.bmiHeader.biPlanes      = 1;
	bmpInfo.bmiHeader.biBitCount    = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage   = width * height * 4;

	HBITMAP bmp = CreateDIBSection(NULL, &bmpInfo, DIB_RGB_COLORS, NULL, NULL, 0);
	mdc         = CreateCompatibleDC(NULL);
	oldBmp      = (HBITMAP)SelectObject(mdc, bmp);
}

MDC::~MDC()
{
	DeleteObject(SelectObject(mdc, oldBmp));
	DeleteDC(mdc);
}

HDC MDC::getDC()
{
	return mdc;
}
