// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "HookDesktop.h"
#include "BufferedRendering.h"
#include "CDAPIModule.h"
using namespace cd;


namespace
{
#define InitModule(module) \
	if (!module::GetInstance().IsReady()) \
	{ \
		MessageBox(NULL, _T(#module) _T("初始化失败！"), _T("CustomDesktop"), MB_ICONERROR); \
		return false; \
	}

	bool InitModules()
	{
		InitModule(HookDesktop)
		InitModule(BufferedRendering)
		InitModule(CDAPIModule)
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
		if (!InitModules())
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

