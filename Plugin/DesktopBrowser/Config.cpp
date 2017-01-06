#include "stdafx.h"
#include "Config.h"
#include <CDAPI.h>


Config g_config;


Config::Config()
{
	LoadConfig((cd::GetPluginDir() + L"\\Data\\DesktopBrowser.ini").c_str());
}

void Config::LoadConfig(LPCWSTR path)
{
	m_homePage.resize(2083);
	GetPrivateProfileStringW(APPNAME, L"HomePage", L"https://github.com/xfgryujk/WinCustomDesktop", (LPWSTR)m_homePage.c_str(), 
		(DWORD)m_homePage.size(), path);
	m_homePage.resize(wcslen(m_homePage.c_str()));
}
