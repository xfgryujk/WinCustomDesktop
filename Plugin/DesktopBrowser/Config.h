#pragma once
#include <string>


static const TCHAR APPNAME[] = _T("DesktopBrowser");

class Config final
{
public:
	std::wstring m_homePage;


	Config();
	void LoadConfig(LPCWSTR path);
};

extern Config g_config;
