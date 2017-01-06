#pragma once
#include "Browser.h"
#include <memory>


class DesktopBrowser final
{
public:
	DesktopBrowser(HMODULE hModule);

private:
	HMODULE m_module;

	std::unique_ptr<Browser> m_browser;


	bool OnPostDrawBackground(HDC& hdc);
};
