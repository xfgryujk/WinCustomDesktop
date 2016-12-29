// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "Global.h"
#include "HookDesktop.h"
#include "BufferedRendering.h"
#include "CDAPIModule.h"
#include "PluginManager.h"
using namespace cd;


namespace
{
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

