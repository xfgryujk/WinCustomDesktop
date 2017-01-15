#pragma once
#include <string>


static const TCHAR APPNAME[] = _T("MaskDesktop");

class Config final
{
public:
	std::wstring m_imagePath;
	int m_size = 100;


	Config();
	void LoadConfig(LPCWSTR path);
};

extern Config g_config;
