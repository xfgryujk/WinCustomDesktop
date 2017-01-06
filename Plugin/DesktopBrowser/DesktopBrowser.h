#pragma once
#include "Browser.h"
#include <memory>
#include <thread>


class DesktopBrowser final
{
public:
	DesktopBrowser(HMODULE hModule);
	~DesktopBrowser();

private:
	HMODULE m_module;

	std::unique_ptr<Browser> m_browser;
	std::unique_ptr<std::thread> m_redrawThread;
	bool m_runThreadFlag = true;
	bool m_pauseFlag = false;


	bool OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& res);
	bool OnPostDrawBackground(HDC& hdc);
};
