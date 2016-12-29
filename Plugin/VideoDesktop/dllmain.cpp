// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <memory>
#include "VideoDesktop.h"


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
		break;

	case DLL_PROCESS_DETACH:
		g_videoDesktop = nullptr;
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

