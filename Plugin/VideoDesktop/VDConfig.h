#pragma once
#include <string>


static const TCHAR APPNAME[] = _T("VideoDesktop");

class VDConfig final
{
public:
	std::wstring m_videoPath;
	int m_volume = 100;
	BOOL m_antiAliasing = FALSE;


	VDConfig();
	void LoadConfig(LPCWSTR path);
};

extern VDConfig g_config;
