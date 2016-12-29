#include "stdafx.h"
#include "DesktopInfo.h"

namespace cd
{
	CDesktopInfo& g_desktopInfo = CDesktopInfo::GetInstance();


	bool CDesktopInfo::Init()
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

	CDesktopInfo::CDesktopInfo()
	{
		Init();
	}
}
