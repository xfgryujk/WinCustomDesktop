#pragma once


static const TCHAR APPNAME[] = _T("WIMC");

class Config final
{
public:
	int m_nCursors = 50;


	Config();
	void LoadConfig(LPCWSTR path);
};

extern Config g_config;
