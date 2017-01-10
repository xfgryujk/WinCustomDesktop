// CustomDesktop.cpp : 定义 DLL 的初始化例程。
#include "stdafx.h"
#include "CustomDesktop.h"
#include <Dbghelp.h>
#include <CDEvents.h>

#include "Global.h"
#include "HookDesktop.h"
#include "BufferedRendering.h"
#include "CDAPIModule.h"
#include "PluginManager.h"
#include "CheckCovered.h"
#include "TrayMenu.h"


cd::CCustomDesktopApp theApp;


namespace cd
{
	BOOL CCustomDesktopApp::InitInstance()
	{
		m_oldExceptionHandler = SetUnhandledExceptionFilter(ExceptionHandler);
#ifdef _DEBUG
		AllocConsole();
		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE);
#endif

		return InitModules(m_hInstance);
	}

	int CCustomDesktopApp::ExitInstance()
	{
		_RPTF0(_CRT_WARN, "CustomDesktop ExitInstance\n");

#ifdef _DEBUG
		FreeConsole();
#endif
		SetUnhandledExceptionFilter(m_oldExceptionHandler);

		return 0;
	}


#define InitModule(module) \
	if (!module::GetInstance().IsReady()) \
	{ \
		MessageBox(g_global.m_topWnd, _T(#module) _T("初始化失败！"), _T("CustomDesktop"), MB_ICONERROR); \
		return false; \
	}

	bool CCustomDesktopApp::InitModules(HMODULE hModule)
	{
		g_global.m_cdModule = hModule;
		// 取dll路径
		g_global.m_cdDir.resize(MAX_PATH);
		GetModuleFileNameW(hModule, &g_global.m_cdDir.front(), (DWORD)g_global.m_cdDir.size());
		g_global.m_cdDir.resize(wcslen(g_global.m_cdDir.c_str()));
		size_t pos = g_global.m_cdDir.rfind(L'\\');
		if (pos != std::string::npos)
			g_global.m_cdDir.resize(pos + 1);

		g_fileListWndProcEvent.AddListener(std::bind(&CCustomDesktopApp::OnFileListWndProc, this, std::placeholders::_1, std::placeholders::_2,
			std::placeholders::_3, std::placeholders::_4));

		InitModule(BufferedRendering)
		InitModule(HookDesktop)
		InitModule(CDAPIModule)
		InitModule(PluginManager)
		InitModule(CheckCovered)
		InitModule(TrayMenu)

		return true;
	}

	// 处理准备卸载的消息
	bool CCustomDesktopApp::OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& res)
	{
		if (message != WM_PREUNLOAD)
			return true;

		// 卸载hook防止崩溃
		_RPTF0(_CRT_WARN, "HookDesktop::GetInstance().Uninit();\n");
		HookDesktop::GetInstance().Uninit();

		CheckCovered::GetInstance().Uninit();

		_RPTF0(_CRT_WARN, "g_preUnloadEvent();\n");
		g_preUnloadEvent();
		// 卸载本模块之前要释放所有插件否则卸载不掉
		_RPTF0(_CRT_WARN, "PluginManager::GetInstance().UnloadAll();\n");
		PluginManager::GetInstance().UnloadAll();

		// 卸载GDI+
		_RPTF0(_CRT_WARN, "BufferedRendering::GetInstance().Uninit();\n");
		BufferedRendering::GetInstance().Uninit();

		res = 1;
		return false;
	}


	// 异常处理
	LONG WINAPI CCustomDesktopApp::ExceptionHandler(_EXCEPTION_POINTERS* ExceptionInfo)
	{
		HANDLE file = CreateFileW((g_global.m_cdDir + L"exception.dmp").c_str(), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file != INVALID_HANDLE_VALUE)
		{
			MINIDUMP_EXCEPTION_INFORMATION einfo;
			einfo.ThreadId = GetCurrentThreadId();
			einfo.ExceptionPointers = ExceptionInfo;
			einfo.ClientPointers = FALSE;
			MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file, MiniDumpWithIndirectlyReferencedMemory,
				&einfo, NULL, NULL);
			CloseHandle(file);
		}
		return EXCEPTION_EXECUTE_HANDLER;
	}
}
