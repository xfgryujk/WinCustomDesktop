// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <memory>
#include "VideoDesktop.h"
#include <CDEvents.h>


std::unique_ptr<VideoDesktop> g_videoDesktop;


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_videoDesktop = std::make_unique<VideoDesktop>(hModule);
		cd::g_preUnloadEvent.AddListener([]{ g_videoDesktop = nullptr; return true; }, hModule);
		break;

	case DLL_PROCESS_DETACH:
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

