#pragma once
#include <string>


static const TCHAR APPNAME[] = _T("VideoDesktop");

class Config final
{
public:
	std::wstring m_videoPath;
	int m_volume = 100;


	Config();
	void LoadConfig(LPCWSTR path);
};

extern Config g_config;
