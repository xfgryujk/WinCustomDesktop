#include "stdafx.h"
#include "CursorRenderer.h"


void CursorRenderer::DrawCursor(HDC hdc, HCURSOR cursor, int x, int y)
{
	auto& cache = GetCursorCache(cursor);
	if (!cache.img.IsNull())
		cache.img.AlphaBlend(hdc, x - cache.hotSpot.x, y - cache.hotSpot.y);
}

CursorRenderer::CursorCache& CursorRenderer::GetCursorCache(HCURSOR cursor)
{
	auto it = m_cursorCache.find(cursor);
	if (it != m_cursorCache.end() && !it->second.img.IsNull())
		return it->second;

	// 没有缓存，创建一个
	auto& cache = m_cursorCache[cursor];

	ICONINFO info;
	GetIconInfo(cursor, &info);
	cache.hotSpot = { info.xHotspot, info.yHotspot };

	GetCursorImage(info, cache.img);

	if (info.hbmColor != NULL)
		DeleteObject(info.hbmColor);
	if (info.hbmMask != NULL)
		DeleteObject(info.hbmMask);
	return cache;
}

bool CursorRenderer::GetCursorImage(ICONINFO& info, CImage& img)
{
	CImage bufferImg;
	// 先尝试彩色图
	if (!GetCursorColorImage(info.hbmColor, info.hbmMask, bufferImg))
	{
		// 单色图
		if (!GetCursorMonoImage(info.hbmMask, bufferImg))
			return false;
	}

	int width = GetSystemMetrics(SM_CXCURSOR);
	int height = GetSystemMetrics(SM_CYCURSOR);
	img.Create(width, height, 32, CImage::createAlphaChannel);
	bufferImg.AlphaBlend(img.GetDC(), 0, 0, width, height, 0, 0, bufferImg.GetWidth(), bufferImg.GetHeight());
	img.ReleaseDC();
	return true;
}

bool CursorRenderer::GetCursorColorImage(HBITMAP color, HBITMAP mask, CImage& img)
{
	BITMAP colorBmp;
	std::unique_ptr<BYTE[]> colorData;
	// color必须是32位图
	if (color == NULL || !GetBmpData(color, colorBmp, colorData) || colorBmp.bmBitsPixel != 32)
		return false;

	BITMAP maskBmp;
	std::unique_ptr<BYTE[]> maskData;
	// 有掩码
	if (GetBmpData(mask, maskBmp, maskData))
	{
		// 判断color有没有alpha通道
		int colorSize = colorBmp.bmWidth * colorBmp.bmHeight;
		for (int i = 0; i < colorSize; i++)
		{
			if (colorData[i * 4 + 3] != 0)
				goto NoMask;
		}
		// 没有alpha通道则用mask（与掩码）当alpha通道
		for (int i = 0; i < colorSize; i++)
			colorData[i * 4 + 3] = (GetMonoBmpBit(maskData.get(), i) != 0 ? 255 : 0);
	}

NoMask:
	// 复制到img
	img.Create(colorBmp.bmWidth, colorBmp.bmHeight, 32, CImage::createAlphaChannel);
	for (int y = 0; y < colorBmp.bmHeight; y++)
	{
		BYTE* imgData = (BYTE*)img.GetPixelAddress(0, y);
		memcpy(imgData, &colorData[y * colorBmp.bmWidthBytes], colorBmp.bmWidthBytes);
	}

	return true;
}

bool CursorRenderer::GetCursorMonoImage(HBITMAP mask, CImage& img)
{
	BITMAP maskBmp;
	std::unique_ptr<BYTE[]> maskData;
	if (mask == NULL || !GetBmpData(mask, maskBmp, maskData))
		return false;

	// 单色图标mask上半部分是与掩码，下半部分是异或掩码
	maskBmp.bmHeight /= 2;
	BYTE* xorMask = maskData.get() + maskBmp.bmHeight * maskBmp.bmWidthBytes;

	img.Create(maskBmp.bmWidth, maskBmp.bmHeight, 32, CImage::createAlphaChannel);
	for (int y = 0; y < maskBmp.bmHeight; y++)
	{
		BYTE* imgData = (BYTE*)img.GetPixelAddress(0, y);
		for (int x = 0; x < maskBmp.bmWidth; x++)
		{
			if (GetMonoBmpBit(maskData.get(), x * y) == 0) // 与掩码为0，alpha为异或掩码取反
				imgData[x * 4 + 3] = (GetMonoBmpBit(xorMask, x * y) != 0 ? 0 : 255);
			else // 与掩码为1，alpha为255，RGB为异或掩码
				*(DWORD*)&imgData[x * 4] = (GetMonoBmpBit(xorMask, x * y) != 0 ? 0xFFFFFFFF : 0xFF000000);
		}
	}

	return true;
}

bool CursorRenderer::GetBmpData(HBITMAP hbmp, BITMAP& bmp, std::unique_ptr<BYTE[]>& data)
{
	if (GetObject(hbmp, sizeof(bmp), &bmp) == 0)
		return false;

	LONG size = bmp.bmWidth * bmp.bmHeight * bmp.bmBitsPixel / 8;
	data.reset(new BYTE[size]);
	GetBitmapBits(hbmp, size, data.get());
	return true;
}

int CursorRenderer::GetMonoBmpBit(const BYTE* data, int index)
{
	// 单色图一位代表一个像素
	return data[index / 8] >> (7 - index % 8) & 1;
}
