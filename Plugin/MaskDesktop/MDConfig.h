#pragma once
#include <string>


static const TCHAR APPNAME[] = _T("MaskDesktop");

class MDConfig final
{
public:
	std::wstring m_imagePath;
	int m_size = 100;


	MDConfig();
	void LoadConfig(LPCWSTR path);
};

extern MDConfig g_config;
