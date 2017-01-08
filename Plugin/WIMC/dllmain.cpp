// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <memory>
#include "WIMC.h"
#include <CDEvents.h>


std::unique_ptr<WIMC> g_wimc;


BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_wimc = std::make_unique<WIMC>(hModule);
		cd::g_preUnloadEvent.AddListener([]{ g_wimc = nullptr; return true; }, hModule);
		break;

	case DLL_PROCESS_DETACH:
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

