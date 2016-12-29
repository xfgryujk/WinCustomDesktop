#pragma once
#include "Singleton.h"
#include <string>


namespace cd
{
	class Global final : public Singleton<Global>
	{
		DECL_SINGLETON(Global);
	public:
		HWND m_topWnd = NULL;
		HWND m_parentWnd = NULL;
		HWND m_fileListWnd = NULL;

		SIZE m_wndSize;

		HMODULE m_cdModule = NULL;
		std::wstring m_cdDir;


		bool Init();

	private:
		Global();
		~Global() = default;
	};

	extern Global& g_global;
}
