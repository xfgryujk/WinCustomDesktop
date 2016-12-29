#pragma once
#include "Singleton.h"


namespace cd
{
	class CDesktopInfo final : public Singleton<CDesktopInfo>
	{
		DECL_SINGLETON(CDesktopInfo);
	public:
		HWND m_topWnd = NULL;
		HWND m_parentWnd = NULL;
		HWND m_fileListWnd = NULL;

		SIZE m_wndSize;

		bool Init();

	private:
		CDesktopInfo();
		~CDesktopInfo() = default;
	};

	extern CDesktopInfo& g_desktopInfo;
}
