// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "AnimatedDesktop.h"
#include "MaskDesktop.h"


CCustomDesktop* g_customDesktop = NULL;


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_customDesktop = new CMaskDesktop();
		g_customDesktop->Init();
		break;

	case DLL_PROCESS_DETACH:
		if (g_customDesktop != NULL)
			delete g_customDesktop;
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

