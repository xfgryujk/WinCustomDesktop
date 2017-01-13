#include "stdafx.h"
#include "CheckCovered.h"
#include <CDEvents.h>
#include <CDAPI.h>
#ifdef _WIN64
#include <Dwmapi.h>
#endif


namespace cd
{
	CheckCovered::CheckCovered()
	{
		Init();
	}

	CheckCovered::~CheckCovered()
	{
		Uninit();
	}
	
	bool CheckCovered::Init()
	{
		m_runThreadFlag = true;
		ExecInMainThread([this]{ m_thread = std::make_unique<std::thread>(&CheckCovered::CheckCoveredThread, this); });
		return true;
	}

	bool CheckCovered::Uninit()
	{
		m_runThreadFlag = false;
		if (m_thread != nullptr && m_thread->joinable())
			m_thread->join();
		m_thread = nullptr;
		return true;
	}


	void CheckCovered::CheckCoveredThread()
	{
		while (m_runThreadFlag)
		{
			if (IsDesktopCovered())
			{
				if (!m_isCovered)
				{
					m_isCovered = true;
					ExecInMainThread([]{ g_desktopCoveredEvent(); });

#ifdef _DEBUG
					WCHAR windowName[100], className[100];
					GetWindowTextW(m_coveredByHwnd, windowName, _countof(windowName));
					GetClassNameW(m_coveredByHwnd, className, _countof(className));
					_RPTFW2(_CRT_WARN, L"桌面被 %s (%s) 遮挡\n", windowName, className);
#endif
				}
			}
			else
			{
				if (m_isCovered)
				{
					m_isCovered = false;
					ExecInMainThread([]{ g_desktopUncoveredEvent(); });

					_RPTF0(_CRT_WARN, "桌面从被遮挡恢复\n");
				}
			}

			for (int i = 0; i < 10; i++)
			{
				if (!m_runThreadFlag)
					break;
				Sleep(100);
			}
		}
	}

	bool CheckCovered::IsDesktopCovered()
	{
		m_coveredByHwnd = NULL;

		EnumWindows([](HWND hwnd, LPARAM pCoveredByHwnd)->BOOL{
#ifdef _WIN64
			// 对于win10 app，不能用IsWindowVisible判断是否可见
			BOOL isCloaked = FALSE;
			DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &isCloaked, sizeof(isCloaked));
			if (isCloaked)
				return TRUE;
#endif

			// 有最大化的窗口而且可见则被遮挡（最小化也是不可见）
			if (IsZoomed(hwnd) && IsWindowVisible(hwnd))
			{
				*(HWND*)pCoveredByHwnd = hwnd;
				return FALSE;
			}
			return TRUE;
		}, (LPARAM)&m_coveredByHwnd);

		return m_coveredByHwnd != NULL;
	}
}
