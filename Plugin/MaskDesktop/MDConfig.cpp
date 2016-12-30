#include "stdafx.h"
#include "MDConfig.h"
#include <CDAPI.h>


MDConfig g_config;


MDConfig::MDConfig()
{
	LoadConfig((cd::GetPluginDir() + L"\\Data\\MaskDesktop.ini").c_str());
}

void MDConfig::LoadConfig(LPCWSTR path)
{
	m_imagePath = cd::GetPluginDir() + L"\\Data\\Mask.png";
	m_size = GetPrivateProfileIntW(APPNAME, L"Size", m_size, path);
}
