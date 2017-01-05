#pragma once
#include <map>
#include <atlimage.h>
#include <memory>


class CursorRenderer
{
public:
	void DrawCursor(HDC hdc, HCURSOR cursor, int x, int y);

private:
	struct CursorCache
	{
		CImage img;
		POINT hotSpot;
	};

	std::map<HCURSOR, CursorCache> m_cursorCache;


	CursorCache& GetCursorCache(HCURSOR cursor);
	bool GetCursorImage(ICONINFO& info, CImage& img);
	bool GetCursorColorImage(HBITMAP color, HBITMAP mask, CImage& img);
	bool GetCursorMonoImage(HBITMAP mask, CImage& img);

	bool GetBmpData(HBITMAP hbmp, BITMAP& bmp, std::unique_ptr<BYTE[]>& data);
	int GetMonoBmpBit(const BYTE* data, int index);
};
