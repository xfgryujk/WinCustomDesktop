#pragma once
#include <vector>


class WIMC final
{
public:
	WIMC(HMODULE hModule);

private:
	HMODULE m_module;


	bool OnPostDrawIcon(HDC& hdc);

	bool OnAppendTrayMenu(HMENU menu);
	bool OnChooseMenuItem(UINT menuID);


	struct FakeCursor
	{
		POINT origin; // 极坐标原点
		float angle; // 与真实鼠标极坐标的角度差

		FakeCursor();
		// 参数：真实鼠标的极坐标
		void Draw(HDC hdc, HCURSOR cursor, float cursorDistance, float cursorAngle, int width, int height);
	};

	// 真实鼠标的极坐标原点
	POINT m_cursorOrigin;
	std::vector<FakeCursor> m_fakeCursors;

	const UINT m_menuID;
};
