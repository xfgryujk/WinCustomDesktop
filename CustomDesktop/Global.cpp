#include "stdafx.h"
#include "Global.h"

namespace cd
{
	Global& g_global = Global::GetInstance();


	bool Global::Init()
	{
		m_topWnd = FindWindow(_T("Progman"), _T("Program Manager"));
		m_parentWnd = FindWindowEx(m_topWnd, NULL, _T("SHELLDLL_DefView"), _T(""));
		m_fileListWnd = FindWindowEx(m_parentWnd, NULL, _T("SysListView32"), _T("FolderView"));
		if (m_fileListWnd == NULL)
			return false;

		RECT rect;
		if (!GetClientRect(m_fileListWnd, &rect))
			rect = {};
		m_wndSize = { rect.right - rect.left, rect.bottom - rect.top };

		return true;
	}

	Global::Global()
	{
		Init();
	}
}
