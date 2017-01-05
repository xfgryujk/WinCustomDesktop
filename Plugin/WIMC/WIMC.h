#pragma once
#include <vector>
#include "CursorRenderer.h"


class WIMC final
{
public:
	WIMC(HMODULE hModule);

private:
	HMODULE m_module;


	bool OnPostDrawIcon(HDC& hdc);


	struct FakeCursor
	{
		POINT origin; // 极坐标原点
		float angle; // 与真实鼠标极坐标的角度差

		FakeCursor();
		// 参数：真实鼠标的极坐标
		void Draw(HDC hdc, HCURSOR cursor, float cursorDistance, float cursorAngle, CursorRenderer& renderer);
	};

	// 真实鼠标的极坐标原点
	POINT m_cursorOrigin;
	std::vector<FakeCursor> m_fakeCursors;
	CursorRenderer m_renderer;
};
