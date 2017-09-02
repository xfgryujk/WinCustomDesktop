// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <memory>
#include "MaskDesktop.h"
#include <CDEvents.h>


std::unique_ptr<MaskDesktop> g_maskDesktop;


BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_maskDesktop = std::make_unique<MaskDesktop>(hModule);
		cd::g_preUnloadEvent.AddListener([]{ g_maskDesktop = nullptr; }, hModule);
		break;

	case DLL_PROCESS_DETACH:
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

