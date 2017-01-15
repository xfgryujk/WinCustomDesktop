#include "stdafx.h"
#include "Config.h"
#include <CDAPI.h>


Config g_config;


Config::Config()
{
	LoadConfig((cd::GetPluginDir() + L"\\Data\\MaskDesktop.ini").c_str());
}

void Config::LoadConfig(LPCWSTR path)
{
	m_imagePath = cd::GetPluginDir() + L"\\Data\\Mask.png";
	m_size = GetPrivateProfileIntW(APPNAME, L"Size", m_size, path);
}
