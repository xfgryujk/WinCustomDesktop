#include "stdafx.h"
#include "Config.h"
#include <CDAPI.h>


Config g_config;


Config::Config()
{
	LoadConfig((cd::GetPluginDir() + L"\\Data\\WIMC.ini").c_str());
}

void Config::LoadConfig(LPCWSTR path)
{
	m_nCursors = GetPrivateProfileIntW(APPNAME, L"NumberOfCursors", m_nCursors, path);
}
