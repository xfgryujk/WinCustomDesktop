#pragma once
#include "CDCommon.h"
#include "EventHelper.h"


namespace cd
{
    extern CD_API PreEvent<HDC>                         g_drawBackgroundEvent;
    extern CD_API PreEvent<LPPAINTSTRUCT, HDC&>         g_fileListBeginPaintEvent;        // 参数, 返回值
    extern CD_API PostEvent<LPPAINTSTRUCT>              g_fileListEndPaintEvent;          // 参数
    
    extern CD_API PreEvent<int, int>                    g_fileListWndSizeEvent;           // width, height
    extern CD_API PreEvent<UINT, WPARAM, LPARAM>	    g_fileListWndProcEvent;		      // message, wParam, lParam
    extern CD_API PreEvent<UINT, WPARAM, LPARAM>	    g_parentWndProcEvent;		      // message, wParam, lParam
}
