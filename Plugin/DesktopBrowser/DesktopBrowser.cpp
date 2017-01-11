#include "stdafx.h"
#include "DesktopBrowser.h"
#include <CDEvents.h>
#include <CDAPI.h>
#include "Config.h"
#include <CommCtrl.h>


DesktopBrowser::DesktopBrowser(HMODULE hModule) : 
	m_module(hModule)
{
	cd::g_fileListWndProcEvent.AddListener(std::bind(&DesktopBrowser::OnFileListWndProc, this, std::placeholders::_1,
		std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), m_module);
	cd::g_postDrawBackgroundEvent.AddListener(std::bind(&DesktopBrowser::OnPostDrawBackground, this, std::placeholders::_1), m_module);
	cd::g_fileListWndSizeEvent.AddListener([this](int width, int height){
		if (m_browser == nullptr)
			return true;
		RECT pos = { 0, 0, width, height };
		m_browser->SetPos(pos);
		return true;
	}, m_module);
	cd::g_preDrawBackgroundEvent.AddListener([](HDC&){ return false; }, m_module);
	cd::g_desktopCoveredEvent.AddListener([this]{ m_pauseFlag = true; return true; }, m_module);
	cd::g_desktopUncoveredEvent.AddListener([this]{ m_pauseFlag = false; return true; }, m_module);

	cd::ExecInMainThread([this]{
		SIZE size;
		cd::GetDesktopSize(size);
		RECT pos = { 0, 0, size.cx, size.cy };
		HRESULT hr;
		m_browser = std::make_unique<Browser>(cd::GetParentHwnd(), pos, hr);
		if (FAILED(hr))
		{
			MessageBox(cd::GetTopHwnd(), _T("加载浏览器失败！"), APPNAME, MB_ICONERROR);
			m_browser = nullptr;
			return;
		}
		m_browser->Navigate(g_config.m_homePage.c_str());

		// 创建一个线程以30fps速率刷新，没什么好办法了
		m_redrawThread = std::make_unique<std::thread>([this]{
			while (m_runThreadFlag)
			{
				if (!m_pauseFlag)
					cd::RedrawDesktop();
				Sleep(33);
			}
		});
	});
	
	cd::RedrawDesktop();
}

DesktopBrowser::~DesktopBrowser()
{
	m_runThreadFlag = false;
	if (m_redrawThread != nullptr && m_redrawThread->joinable())
		m_redrawThread->join();
	m_redrawThread = nullptr;
}


bool DesktopBrowser::OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& res)
{
	switch (message)
	{
	case WM_NCHITTEST:
	{
		LVHITTESTINFO hitTestInfo;
		hitTestInfo.pt = { MAKEPOINTS(lParam).x, MAKEPOINTS(lParam).y };
		if (ListView_HitTest(cd::GetFileListHwnd(), &hitTestInfo) != -1)
			return true;
		// 设置图标以外的地方不属于文件列表窗口
		res = HTTRANSPARENT;
		return false;
	}
	}
	return true;
}

bool DesktopBrowser::OnPostDrawBackground(HDC& hdc)
{
	if (m_browser == nullptr)
		return true;
	SIZE size;
	cd::GetDesktopSize(size);
	RECT rect = { 0, 0, size.cx, size.cy };
	m_browser->Draw(hdc, rect);
	return true;
}
