#pragma once
#include <string>


static const TCHAR APPNAME[] = _T("VideoDesktop");

class VDConfig final
{
public:
	std::wstring m_videoPath;


	VDConfig();
	void LoadConfig(LPCWSTR path);
};

extern VDConfig g_config;
