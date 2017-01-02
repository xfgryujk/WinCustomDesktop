#pragma once
#include "Singleton.h"
#include <string>
#include <vector>


namespace cd
{
	class Global final : public Singleton<Global>
	{
		DECL_SINGLETON(Global);
	public:
		// 窗口相关信息
		HWND m_topWnd = NULL;
		HWND m_parentWnd = NULL;
		HWND m_fileListWnd = NULL;
		SIZE m_wndSize;
		SIZE m_screenSize;

		// 原窗口过程
		WNDPROC m_oldFileListWndProc = NULL;
		WNDPROC m_oldParentWndProc = NULL;

		// XP有两个comctl32，所以需要获取所有comctl32句柄
		std::vector<HMODULE> m_comctlModules;

		// CustomDesktop相关信息
		HMODULE m_cdModule = NULL;
		std::wstring m_cdDir;

		// 正在调用BeginPaint，用来禁用XP下BeginPaint擦背景
		bool m_isInBeginPaint = false;
		// 需要更新图标层，comctl调用RedrawWindow后为true
		bool m_needUpdateIcon = true;


		bool Init();

	private:
		Global();
		~Global() = default;
	};

	extern Global& g_global;
}
