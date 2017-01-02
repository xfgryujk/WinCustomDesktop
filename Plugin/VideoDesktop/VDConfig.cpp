#include "stdafx.h"
#include "VDConfig.h"
#include <CDAPI.h>


VDConfig g_config;


VDConfig::VDConfig()
{
	LoadConfig((cd::GetPluginDir() + L"\\Data\\VideoDesktop.ini").c_str());
}

void VDConfig::LoadConfig(LPCWSTR path)
{
	std::wstring strBuf;

	strBuf.resize(MAX_PATH);
	GetPrivateProfileStringW(APPNAME, L"VideoPath", L"", (LPWSTR)strBuf.c_str(), (DWORD)strBuf.size(), path);
	strBuf.resize(wcslen(strBuf.c_str()));
	m_videoPath = std::move(strBuf);

	m_volume = GetPrivateProfileIntW(APPNAME, L"Volume", m_volume, path);
}
