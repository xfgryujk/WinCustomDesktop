#pragma once
#include "EventHelper.h"


namespace cd
{
	extern Event<HDC>                         g_onDrawBackgroundEvent;
	extern Event<int, int>                    g_fileListWndSizeEvent;           // width, height
	extern Event<LPPAINTSTRUCT, HDC&>         g_fileListBeginPaintEvent;        // 参数, 返回值
	extern Event<CONST PAINTSTRUCT *>         g_fileListEndPaintEvent;          // 参数
}
