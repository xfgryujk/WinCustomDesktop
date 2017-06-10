#include "stdafx.h"
#include "WIMC.h"
#include <CDEvents.h>
using namespace std::placeholders;
#include <CDAPI.h>
#include "Config.h"
#include <thread>
#include <shellapi.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <random>


WIMC::WIMC(HMODULE hModule) : 
	m_module(hModule),
	m_menuID(cd::GetMenuID())
{
	m_fakeCursors.resize(g_config.m_nCursors);

	cd::g_postDrawIconEvent.AddListener(std::bind(&WIMC::OnPostDrawIcon, this, _1), m_module);
	cd::g_fileListWndProcEvent.AddListener([](UINT message, WPARAM, LPARAM, LRESULT& res){
		if (message == WM_MOUSEMOVE)
			cd::RedrawDesktop();
		return true;
	}, m_module);
	cd::g_appendTrayMenuEvent.AddListener(std::bind(&WIMC::OnAppendTrayMenu, this, _1), m_module);
	cd::g_chooseMenuItemEvent.AddListener(std::bind(&WIMC::OnChooseMenuItem, this, _1), m_module);
	
	cd::RedrawDesktop();
}

bool WIMC::OnPostDrawIcon(HDC& hdc)
{
	CURSORINFO info;
	info.cbSize = sizeof(info);
	if (!GetCursorInfo(&info))
		return true;

	const auto& pos = info.ptScreenPos;
	const float distance = sqrtf(float((pos.x - m_cursorOrigin.x) * (pos.x - m_cursorOrigin.x) 
		+ (pos.y - m_cursorOrigin.y) * (pos.y - m_cursorOrigin.y)));
	const float angle = atan2f(float(pos.y - m_cursorOrigin.y), float(pos.x - m_cursorOrigin.x));

	const int width = GetSystemMetrics(SM_CXCURSOR);
	const int height = GetSystemMetrics(SM_CYCURSOR);

	for (auto& i : m_fakeCursors)
		i.Draw(hdc, info.hCursor, distance, angle, width, height);

	return true;
}


bool WIMC::OnAppendTrayMenu(HMENU menu)
{
	AppendMenu(menu, MF_STRING, m_menuID, APPNAME);
	return true;
}

bool WIMC::OnChooseMenuItem(UINT menuID)
{
	if (menuID != m_menuID)
		return true;

	std::thread([this]{
		SHELLEXECUTEINFOW info = {};
		info.cbSize = sizeof(info);
		info.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC;
		info.lpVerb = L"open";
		info.lpFile = L"notepad.exe";
		std::wstring param = cd::GetPluginDir() + L"\\Data\\WIMC.ini";
		info.lpParameters = param.c_str();
		info.nShow = SW_SHOWNORMAL;
		ShellExecuteExW(&info);

		WaitForSingleObject(info.hProcess, INFINITE);
		CloseHandle(info.hProcess);

		cd::ExecInMainThread([this]{
			Config newConfig;
			if (newConfig.m_nCursors != g_config.m_nCursors)
			{
				m_fakeCursors.clear();
				m_fakeCursors.resize(newConfig.m_nCursors);
			}
			g_config = newConfig;
		});
	}).detach();
	return false;
}


namespace
{
	float GetRandomFloat(float min, float max)
	{
		static std::random_device s_randomDevice;
		return std::uniform_real_distribution<float>(min, max)(s_randomDevice);
	}
}

WIMC::FakeCursor::FakeCursor()
{
	SIZE size;
	cd::GetDesktopSize(size);
	origin.x = static_cast<int>(GetRandomFloat(-size.cx * 0.75f, size.cx * 1.75f));
	origin.y = static_cast<int>(GetRandomFloat(-size.cy * 0.75f, size.cy * 1.75f));
	angle = GetRandomFloat(0.0f, 2.0f * float(M_PI));
}

void WIMC::FakeCursor::Draw(HDC hdc, HCURSOR cursor, float cursorDistance, float cursorAngle, int width, int height)
{
	const auto x = int(origin.x + cursorDistance * cosf(cursorAngle + angle));
	const auto y = int(origin.y + cursorDistance * sinf(cursorAngle + angle));
	// 没有对准热点，不过没什么影响
	DrawIconEx(hdc, x, y, cursor, width, height, 0, NULL, DI_NORMAL);
}
