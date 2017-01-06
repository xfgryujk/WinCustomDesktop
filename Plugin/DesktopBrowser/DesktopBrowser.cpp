#include "stdafx.h"
#include "DesktopBrowser.h"
#include <CDEvents.h>
#include <CDAPI.h>
#include "Config.h"


DesktopBrowser::DesktopBrowser(HMODULE hModule) : 
	m_module(hModule)
{
	cd::g_postDrawBackgroundEvent.AddListener(std::bind(&DesktopBrowser::OnPostDrawBackground, this, std::placeholders::_1), m_module);

	cd::ExecInMainThread([this]{
		SIZE size;
		cd::GetDesktopSize(size);
		m_browser = std::make_unique<Browser>(cd::GetParentHwnd(), size);
		m_browser->Navigate(g_config.m_homePage.c_str());
	});
	
	cd::RedrawDesktop();
}

bool DesktopBrowser::OnPostDrawBackground(HDC& hdc)
{
	SIZE size;
	cd::GetDesktopSize(size);
	RECT rect = { 0, 0, size.cx, size.cy };
	m_browser->Draw(hdc, &rect);
	return true;
}
