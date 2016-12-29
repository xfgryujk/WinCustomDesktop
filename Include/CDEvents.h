#pragma once
#include "CDCommon.h"
#include "EventHelper.h"


namespace cd
{
	extern CD_API PostEvent<>							g_preUnloadEvent;				  // 准备卸载，在处理消息时触发

    extern CD_API PreEvent<HDC>                         g_drawBackgroundEvent;			  // 画桌面背景，目标DC
    extern CD_API PreEvent<LPPAINTSTRUCT, HDC&>         g_fileListBeginPaintEvent;        // 参数, 返回值
    extern CD_API PostEvent<LPPAINTSTRUCT>              g_fileListEndPaintEvent;          // 参数
    
    extern CD_API PreEvent<int, int>                    g_fileListWndSizeEvent;           // width, height
    extern CD_API PreEvent<UINT, WPARAM, LPARAM>	    g_fileListWndProcEvent;		      // message, wParam, lParam
    extern CD_API PreEvent<UINT, WPARAM, LPARAM>	    g_parentWndProcEvent;		      // message, wParam, lParam
}
