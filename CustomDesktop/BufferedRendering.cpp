#include "stdafx.h"
#include "BufferedRendering.h"
#include "Global.h"
#include <CDEvents.h>
#include <CDAPI.h>
#include "HookDesktop.h"


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

		// 创建各种DC
		if (!InitDC())
			return false;

		// 监听事件
		g_fileListWndProcEvent.AddListener(std::bind(&BufferedRendering::OnFileListWndProc, this, std::placeholders::_1, std::placeholders::_2, 
			std::placeholders::_3));
		g_parentWndProcEvent.AddListener(std::bind(&BufferedRendering::OnParentWndProc, this, std::placeholders::_1, std::placeholders::_2,
			std::placeholders::_3));
		g_drawBackgroundEvent.AddListener(std::bind(&BufferedRendering::OnDrawBackground, this, std::placeholders::_1, std::placeholders::_2));
		g_fileListRedrawWindowEvent.AddListener([](CONST RECT*, HRGN, UINT){ g_global.m_needUpdateIcon = true; return true; });
		g_fileListBeginPaintEvent.AddListener(std::bind(&BufferedRendering::OnFileListBeginPaint, this, std::placeholders::_1, std::placeholders::_2));
		g_fileListEndPaintEvent.AddListener(std::bind(&BufferedRendering::OnFileListEndPaint, this, std::placeholders::_1));

		// 获取图标层
		RedrawDesktop();

		m_hasInit = true;
		return true;
	}

	bool BufferedRendering::Uninit()
	{
		if (!m_hasInit)
			return true;

		if (!m_bufferImg.IsNull())
		{
			m_bufferImg.ReleaseDC();
			m_bufferImg.Destroy();
		}
		if (!m_bufferImgBackup.IsNull())
			m_bufferImgBackup.Destroy();
		if (!m_wallpaperImg.IsNull())
			m_wallpaperImg.Destroy();
		if (!m_iconBufferImg.IsNull())
			m_iconBufferImg.Destroy();
		CImage::ReleaseGDIPlus();

		m_hasInit = false;
		return true;
	}

	// 更新各种buffer尺寸
	bool BufferedRendering::InitDC()
	{
		if (!m_bufferImg.IsNull())
		{
			m_bufferImg.ReleaseDC();
			m_bufferImg.Destroy();
		}
		if (!m_bufferImg.Create(g_global.m_wndSize.cx, g_global.m_wndSize.cy, 32, CImage::createAlphaChannel))
			return false;
		m_bufferDC = m_bufferImg.GetDC();

		if (!m_bufferImgBackup.IsNull())
			m_bufferImgBackup.Destroy();
		if (!m_bufferImgBackup.Create(g_global.m_wndSize.cx, g_global.m_wndSize.cy, 32, CImage::createAlphaChannel))
			return false;

		if (!m_iconBufferImg.IsNull())
			m_iconBufferImg.Destroy();
		if (!m_iconBufferImg.Create(g_global.m_wndSize.cx, g_global.m_wndSize.cy, 32, CImage::createAlphaChannel))
			return false;

		InitWallpaperDC();
		return true;
	}

	bool BufferedRendering::InitWallpaperDC()
	{
		// 取壁纸路径
		HKEY key = NULL;
		if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Control Panel\\Desktop"), 0, KEY_READ, &key) != ERROR_SUCCESS)
			return false;
		std::wstring wallpaperPath;
		wallpaperPath.resize(MAX_PATH);
		DWORD type = 0;
		DWORD size = DWORD(wallpaperPath.size() * sizeof(WCHAR));
		if (RegQueryValueExW(key, L"WallPaper", NULL, &type, (LPBYTE)wallpaperPath.c_str(), &size) != ERROR_SUCCESS
			|| type != REG_SZ)
		{
			RegCloseKey(key);
			return false;
		}
		RegCloseKey(key);
		size /= sizeof(WCHAR);
		wallpaperPath.resize(wallpaperPath[size - 1] == L'\0' ? size - 1 : size);

		// 创建壁纸DC
		CImage img;
		if (FAILED(img.Load(wallpaperPath.c_str())))
			return false;
		if (!m_wallpaperImg.IsNull())
		{
			m_wallpaperImg.ReleaseDC();
			m_wallpaperImg.Destroy();
		}
		if (!m_wallpaperImg.Create(g_global.m_screenSize.cx, g_global.m_screenSize.cy, 32))
			return false;
		img.Draw(m_wallpaperImg.GetDC(), 0, 0, g_global.m_screenSize.cx, g_global.m_screenSize.cy);
		m_wallpaperImg.ReleaseDC();
		return true;
	}


	// 处理size、接管paint
	bool BufferedRendering::OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_SIZE:
			g_global.m_wndSize = { LOWORD(lParam), HIWORD(lParam) };
			g_global.m_screenSize = { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
			InitDC();

			g_fileListWndSizeEvent(g_global.m_wndSize.cx, g_global.m_wndSize.cy);
			break;

		case WM_PAINT:
		{
			// 交给原窗口过程
			if (g_global.m_needUpdateIcon)
				return true;
			
			PAINTSTRUCT paint;
			g_global.m_isInBeginPaint = true;
			HDC hdc = BeginPaint(g_global.m_fileListWnd, &paint);
			g_global.m_isInBeginPaint = false;
			g_fileListBeginPaintEvent(&paint, hdc);

			// 背景层
			SendMessage(g_global.m_parentWnd, WM_ERASEBKGND, (WPARAM)hdc, 0);
			// 图标层
			m_iconBufferImg.AlphaBlend(hdc, 0, 0);

			g_fileListEndPaintEvent(&paint);
			EndPaint(g_global.m_fileListWnd, &paint);
			return false;
		}
		}
		return true;
	}

	// 触发画背景事件
	bool BufferedRendering::OnParentWndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_ERASEBKGND:
			// wParam不一定是m_bufferDC，comctl内部也用了双缓冲

			// 画背景
			if (g_drawBackgroundEvent(m_bufferDC, g_global.m_isInBeginPaint))
				CallWindowProc(HookDesktop::GetInstance().m_oldParentWndProc, g_global.m_parentWnd, message, (WPARAM)m_bufferDC, lParam);

			// 准备更新图标层
			if (g_global.m_needUpdateIcon)
			{
				g_global.m_needUpdateIcon = false;
				m_isUpdatingIcon = true;

				// 背景复制到m_bufferImgBackup
				m_bufferImg.BitBlt(m_bufferImgBackup.GetDC(), 0, 0);
				m_bufferImgBackup.ReleaseDC();

				// wParam、m_bufferImg alpha清0
				RECT rect = { 0, 0, g_global.m_wndSize.cx, g_global.m_wndSize.cy };
				FillRect((HDC)wParam, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
				void* imgData = m_bufferImg.GetPixelAddress(0, m_bufferImg.GetHeight() - 1);
				memset(imgData, 0, m_bufferImg.GetWidth() * m_bufferImg.GetHeight() * m_bufferImg.GetBPP() / 8);
			}

			// 不需要上级CallWindowProc了
			return false;
		}
		return true;
	}

	// 禁用XP下BeginPaint擦背景、自己画背景
	bool BufferedRendering::OnDrawBackground(HDC& hdc, bool isInBeginPaint)
	{
		// 只有XP下BeginPaint才会擦除背景
		if (isInBeginPaint)
		{
			hdc = m_bufferDC;
			// XP下禁用BeginPaint擦背景后画不上去了，只好自己画背景到缓冲DC
			//m_wallpaperImg.AlphaBlend(m_bufferDC, 0, 0);
			m_wallpaperImg.BitBlt(m_bufferDC, 0, 0, g_global.m_wndSize.cx, g_global.m_wndSize.cy, 0, 0);
			return false;
		}
		return true;
	}

	// 把窗口DC替换成缓冲DC
	bool BufferedRendering::OnFileListBeginPaint(LPPAINTSTRUCT lpPaint, HDC& res)
	{
		if (res != NULL)
		{
			m_originalDC = res;
			res = lpPaint->hdc = m_bufferDC;
		}
		return true;
	}

	// 把缓冲DC复制回窗口DC
	bool BufferedRendering::OnFileListEndPaint(LPPAINTSTRUCT lpPaint)
	{
		if (lpPaint->hdc != NULL)
		{
			lpPaint->hdc = m_originalDC;

			int x = lpPaint->rcPaint.left;
			int y = lpPaint->rcPaint.top;
			int width = lpPaint->rcPaint.right - lpPaint->rcPaint.left;
			int height = lpPaint->rcPaint.bottom - lpPaint->rcPaint.top;

			if (m_isUpdatingIcon) // 现在缓冲DC是图标层
			{
				m_isUpdatingIcon = false;

				// 取图标层
				m_bufferImg.BitBlt(m_iconBufferImg.GetDC(), x, y, width, height, x, y);
				m_iconBufferImg.ReleaseDC();

				// 背景
				m_bufferImgBackup.BitBlt(m_bufferDC, x, y, width, height, x, y);
				// 图标
				m_iconBufferImg.AlphaBlend(m_bufferDC, x, y, width, height, x, y, width, height);
			}

			m_bufferImg.BitBlt(m_originalDC, x, y, width, height, x, y);
		}
		return true;
	}
}
