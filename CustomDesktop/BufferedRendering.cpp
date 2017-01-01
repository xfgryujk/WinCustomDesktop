#include "stdafx.h"
#include "BufferedRendering.h"
#include "Global.h"
#include <CDEvents.h>
#include <CDAPI.h>


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
		if (!m_bufferImg.Create(g_global.m_wndSize.cx, g_global.m_wndSize.cy, 24))
			return false;
		m_bufferDC = m_bufferImg.GetDC();

		// 创建壁纸DC
		InitWallpaperDC();

		// 监听事件
		g_fileListWndSizeEvent.AddListener(std::bind(&BufferedRendering::OnFileListWndSize, this, std::placeholders::_1, std::placeholders::_2));
		g_fileListBeginPaintEvent.AddListener(std::bind(&BufferedRendering::OnFileListBeginPaint, this, std::placeholders::_1, std::placeholders::_2));
		g_fileListEndPaintEvent.AddListener(std::bind(&BufferedRendering::OnFileListEndPaint, this, std::placeholders::_1));
		g_drawBackgroundEvent.AddListener(std::bind(&BufferedRendering::OnDrawBackground, this, std::placeholders::_1, std::placeholders::_2));

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
		if (!m_wallpaperImg.IsNull())
		{
			m_wallpaperImg.ReleaseDC();
			m_wallpaperImg.Destroy();
		}
		CImage::ReleaseGDIPlus();

		m_hasInit = false;
		return true;
	}

	void BufferedRendering::InitWallpaperDC()
	{
		// 取壁纸路径
		HKEY key = NULL;
		if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Control Panel\\Desktop"), 0, KEY_READ, &key) != ERROR_SUCCESS)
			return;
		std::wstring wallpaperPath;
		wallpaperPath.resize(MAX_PATH);
		DWORD type = 0;
		DWORD size = DWORD(wallpaperPath.size() * sizeof(WCHAR));
		if (RegQueryValueExW(key, L"WallPaper", NULL, &type, (LPBYTE)wallpaperPath.c_str(), &size) != ERROR_SUCCESS
			|| type != REG_SZ)
		{
			RegCloseKey(key);
			return;
		}
		RegCloseKey(key);
		size /= sizeof(WCHAR);
		wallpaperPath.resize(wallpaperPath[size - 1] == L'\0' ? size - 1 : size);

		// 创建壁纸DC
		CImage img;
		img.Load(wallpaperPath.c_str());
		if (!m_wallpaperImg.IsNull())
		{
			m_wallpaperImg.ReleaseDC();
			m_wallpaperImg.Destroy();
		}
		m_wallpaperImg.Create(g_global.m_screenSize.cx, g_global.m_screenSize.cy, 24);
		m_wallpaperDC = m_wallpaperImg.GetDC();
		img.Draw(m_wallpaperDC, 0, 0, g_global.m_screenSize.cx, g_global.m_screenSize.cy);
	}


	bool BufferedRendering::OnFileListWndSize(int width, int height)
	{
		if (!m_bufferImg.IsNull())
		{
			m_bufferImg.ReleaseDC();
			m_bufferImg.Destroy();
		}
		m_bufferImg.Create(width, height, 24);
		m_bufferDC = m_bufferImg.GetDC();
		InitWallpaperDC();
		return true;
	}

	bool BufferedRendering::OnDrawBackground(HDC& hdc, bool isInBeginPaint)
	{
		if (isInBeginPaint)
		{
			hdc = m_bufferDC;
			// XP下禁用BeginPaint擦背景后画不上去了，只好自己画背景到缓冲DC
			m_wallpaperImg.BitBlt(m_bufferDC, 0, 0, g_global.m_wndSize.cx, g_global.m_wndSize.cy, 0, 0);
			return false;
		}
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

	bool BufferedRendering::OnFileListEndPaint(LPPAINTSTRUCT lpPaint)
	{
		if (lpPaint->hdc != NULL)
		{
			m_bufferImg.BitBlt(m_originalDC, lpPaint->rcPaint.left, lpPaint->rcPaint.top, lpPaint->rcPaint.right - lpPaint->rcPaint.left,
				lpPaint->rcPaint.bottom - lpPaint->rcPaint.top, lpPaint->rcPaint.left, lpPaint->rcPaint.top);

			lpPaint->hdc = m_originalDC;
		}
		return true;
	}
}
