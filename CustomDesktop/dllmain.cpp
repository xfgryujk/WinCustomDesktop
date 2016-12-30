// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "Global.h"
#include "HookDesktop.h"
#include "BufferedRendering.h"
#include "CDAPIModule.h"
#include "PluginManager.h"
#include <CDEvents.h>
using namespace cd;


namespace
{
	// 准备卸载的消息
	static const UINT WM_PREUNLOAD = WM_APP + 999;


#define InitModule(module) \
	if (!module::GetInstance().IsReady()) \
	{ \
		MessageBox(NULL, _T(#module) _T("初始化失败！"), _T("CustomDesktop"), MB_ICONERROR); \
		return false; \
	}

	bool InitModules(HMODULE hModule)
	{
		g_global.m_cdModule = hModule;
		// 取dll路径
		g_global.m_cdDir.resize(MAX_PATH);
		GetModuleFileNameW(hModule, (LPWSTR)g_global.m_cdDir.c_str(), (DWORD)g_global.m_cdDir.size());
		g_global.m_cdDir.resize(wcslen(g_global.m_cdDir.c_str()));
		size_t pos = g_global.m_cdDir.rfind(L'\\');
		if (pos != std::string::npos)
			g_global.m_cdDir.resize(pos + 1);

		InitModule(HookDesktop)
		InitModule(BufferedRendering)
		InitModule(CDAPIModule)
		InitModule(PluginManager)

		g_fileListWndProcEvent.AddListener([](UINT message, WPARAM wParam, LPARAM lParam){
			if (message == WM_PREUNLOAD)
			{
				g_preUnloadEvent();
				// 卸载之前要释放所有插件否则卸载不掉
				PluginManager::GetInstance().UnloadAll();
				// 卸载hook防止崩溃
				HookDesktop::GetInstance().Uninit();
				return false;
			}
			return true;
		});

		return true;
	}
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (!InitModules(hModule))
			return FALSE;
		break;

	case DLL_PROCESS_DETACH:
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

