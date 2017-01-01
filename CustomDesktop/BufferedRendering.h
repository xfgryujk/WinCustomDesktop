#pragma once
#include "Singleton.h"
#include <atlimage.h>


namespace cd
{
	class BufferedRendering final : public Singleton<BufferedRendering>
	{
		DECL_SINGLETON(BufferedRendering);
	public:
		bool IsReady() { return m_hasInit; }

		bool Init();
		bool Uninit();

	private:
		bool m_hasInit = false;

		BufferedRendering();
		~BufferedRendering();


		HDC m_originalDC = NULL;
		CImage m_bufferImg;
		HDC m_bufferDC = NULL;
		CImage m_wallpaperImg;
		HDC m_wallpaperDC = NULL;

		void InitWallpaperDC();

		bool OnFileListWndSize(int width, int height);
		bool OnDrawBackground(HDC& hdc, bool isInBeginPaint);
		bool OnFileListBeginPaint(LPPAINTSTRUCT lpPaint, HDC& res);
		bool OnFileListEndPaint(LPPAINTSTRUCT lpPaint);
	};
}
