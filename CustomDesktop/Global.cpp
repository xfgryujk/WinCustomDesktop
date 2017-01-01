#include "stdafx.h"
#include "Global.h"
#include <Psapi.h>

namespace cd
{
	Global& g_global = Global::GetInstance();


	bool Global::Init()
	{
		m_topWnd = FindWindow(_T("Progman"), _T("Program Manager"));
		m_parentWnd = FindWindowEx(m_topWnd, NULL, _T("SHELLDLL_DefView"), _T(""));
		m_fileListWnd = FindWindowEx(m_parentWnd, NULL, _T("SysListView32"), _T("FolderView"));
		if (m_fileListWnd == NULL) return false;

		RECT rect;
		if (!GetClientRect(m_fileListWnd, &rect)) return false;
		m_wndSize = { rect.right - rect.left, rect.bottom - rect.top };
		m_screenSize = { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };

		DWORD size = 0;
		if (!EnumProcessModules(GetCurrentProcess(), NULL, 0, &size)) return false;
		std::vector<HMODULE> modules(size / sizeof(HMODULE));
		if (!EnumProcessModules(GetCurrentProcess(), &modules[0], size, &size)) return false;
		m_comctlModules.clear();
		for (const auto& module : modules)
		{
			TCHAR moduleName[MAX_PATH];
			if (!GetModuleFileName(module, moduleName, _countof(moduleName))) return false;
			if (_tcsicmp(_tcsrchr(moduleName, _T('\\')) + 1, L"comctl32.dll") == 0)
				m_comctlModules.push_back(module);
		}
		if (m_comctlModules.empty()) return false;

		return true;
	}

	Global::Global()
	{
		Init();
	}
}
