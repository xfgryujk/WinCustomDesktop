#pragma once
#include "Singleton.h"
#include <memory>
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


		// XP下不接管桌面绘制。我真是败给XP了...
		bool m_controlRendering = true;

		HDC m_originalDC = NULL;
		CImage m_bufferImg;
		HDC m_bufferDC = NULL;

		CImage m_wallpaperImg, m_bgBackupImg, m_iconBufferImg;
		struct HRGNDeleter
		{
			typedef HRGN pointer;
			void operator() (HRGN rgn) { DeleteObject(rgn); }
		};
		// 背景备份的裁剪区，用来防止多次绘制背景覆盖之前的内容
		std::unique_ptr<HRGN, HRGNDeleter> m_bgBackupClipRgn;
		// m_bufferImg（背景层）复制到m_bgBackupImg了，现在m_bufferImg是图标层
		bool m_isUpdatingIcon = false;


		bool InitDC();
		bool InitWallpaperDC();

		void OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& res, bool& pass);
		void OnParentWndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& res, bool& pass);
		void PostDrawIcon(HDC& hdc);
		void OnFileListBeginPaint(LPPAINTSTRUCT lpPaint, HDC& res);
		void OnFileListEndPaint(LPPAINTSTRUCT lpPaint);
	};
}
