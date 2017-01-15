#include "stdafx.h"
#include "Config.h"
#include <CDAPI.h>


Config g_config;


Config::Config()
{
	LoadConfig((cd::GetPluginDir() + L"\\Data\\VideoDesktop.ini").c_str());
}

void Config::LoadConfig(LPCWSTR path)
{
	m_videoPath.resize(MAX_PATH);
	GetPrivateProfileStringW(APPNAME, L"VideoPath", L"", &m_videoPath.front(), static_cast<DWORD>(m_videoPath.size()), path);
	m_videoPath.resize(wcslen(m_videoPath.c_str()));

	m_volume = GetPrivateProfileIntW(APPNAME, L"Volume", m_volume, path);
}
