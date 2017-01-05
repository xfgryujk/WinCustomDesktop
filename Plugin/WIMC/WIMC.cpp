#include "stdafx.h"
#include "WIMC.h"
#include <CDEvents.h>
#include <CDAPI.h>
#include "Config.h"
#define _USE_MATH_DEFINES
#include <math.h>


WIMC::WIMC(HMODULE hModule) : 
	m_module(hModule)
{
	srand(GetTickCount());
	m_fakeCursors.resize(g_config.m_nCursors);

	cd::g_postDrawIconEvent.AddListener(std::bind(&WIMC::OnPostDrawIcon, this, std::placeholders::_1), m_module);
	cd::g_fileListWndProcEvent.AddListener([](UINT message, WPARAM, LPARAM){
		if (message == WM_MOUSEMOVE)
			cd::RedrawDesktop();
		return true;
	}, m_module);
	
	cd::RedrawDesktop();
}

bool WIMC::OnPostDrawIcon(HDC& hdc)
{
	CURSORINFO info;
	info.cbSize = sizeof(info);
	if (!GetCursorInfo(&info))
		return true;

	POINT& pos = info.ptScreenPos;
	float distance = sqrtf(float((pos.x - m_cursorOrigin.x) * (pos.x - m_cursorOrigin.x) 
		+ (pos.y - m_cursorOrigin.y) * (pos.y - m_cursorOrigin.y)));
	float angle = atan2f(float(pos.y - m_cursorOrigin.y), float(pos.x - m_cursorOrigin.x));

	for (auto& i : m_fakeCursors)
		i.Draw(hdc, info.hCursor, distance, angle, m_renderer);

	return true;
}


static float GetRandomFloat(float min, float max)
{
	return ((float)rand() / (RAND_MAX + 1)) * (max - min) + min;
}

WIMC::FakeCursor::FakeCursor()
{
	SIZE size;
	cd::GetDesktopSize(size);
	origin.x = (int)GetRandomFloat(-size.cx / 2.0f, size.cx * 1.5f);
	origin.y = (int)GetRandomFloat(-size.cy / 2.0f, size.cy * 1.5f);
	angle = GetRandomFloat(0.0f, 2.0f * float(M_PI));
}

void WIMC::FakeCursor::Draw(HDC hdc, HCURSOR cursor, float cursorDistance, float cursorAngle, CursorRenderer& renderer)
{
	int x = int(origin.x + cursorDistance * cosf(cursorAngle + angle));
	int y = int(origin.y + cursorDistance * sinf(cursorAngle + angle));
	renderer.DrawCursor(hdc, cursor, x, y);
}
