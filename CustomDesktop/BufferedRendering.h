#pragma once
#include "Singleton.h"
#include <atlimage.h>


namespace cd
{
	// 接管桌面绘制过程，顺便实现双缓冲
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
		CImage m_bufferImg, m_bufferImgBackup;
		HDC m_bufferDC = NULL;

		CImage m_wallpaperImg;
		CImage m_iconBufferImg;
		// m_bufferImg复制到m_bufferImgBackup了，现在m_bufferImg是图标层
		bool m_isUpdatingIcon = false;

		bool InitDC();
		bool InitWallpaperDC();

		bool OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam);
		bool OnParentWndProc(UINT message, WPARAM wParam, LPARAM lParam);
		bool OnDrawBackground(HDC& hdc, bool isInBeginPaint);
		bool OnFileListBeginPaint(LPPAINTSTRUCT lpPaint, HDC& res);
		bool OnFileListEndPaint(LPPAINTSTRUCT lpPaint);
	};
}
