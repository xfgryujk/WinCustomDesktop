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
	m_videoPath.resize(MAX_PATH);
	GetPrivateProfileStringW(APPNAME, L"VideoPath", L"", &m_videoPath.front(), static_cast<DWORD>(m_videoPath.size()), path);
	m_videoPath.resize(wcslen(m_videoPath.c_str()));

	m_volume = GetPrivateProfileIntW(APPNAME, L"Volume", m_volume, path);
}
