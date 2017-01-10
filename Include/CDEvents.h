#pragma once
#include "CDCommon.h"
#include "EventHelper.h"


namespace cd
{
	// 程序行为的事件
	extern CD_API PostEvent<>							    g_preUnloadEvent;			  // 准备卸载，用来做不能在dllmain完成的卸载，在处理消息时触发
	extern CD_API PostEvent<>							    g_desktopCoveredEvent;		  // 桌面被遮挡了，在处理消息时触发
	extern CD_API PostEvent<>							    g_desktopUncoveredEvent;	  // 桌面从被遮挡恢复，在处理消息时触发
	extern CD_API PreEvent<int, int>                        g_fileListWndSizeEvent;       // 参数：width, height

	// 用户接口的事件
	extern CD_API PreEvent<HMENU>                           g_appendTrayMenuEvent;        // 用来添加托盘菜单，菜单ID应用GetMenuID获取，参数：菜单句柄
	extern CD_API PreEvent<UINT>                            g_chooseMenuItemEvent;        // 用户选择了某菜单项，参数：菜单ID

	// 渲染的事件
	extern CD_API PreEvent<HDC&>                            g_preDrawBackgroundEvent;	  // 画桌面背景前被调用，用来取消画背景节省CPU，参数：目标DC
	extern CD_API PostEvent<HDC&>                           g_postDrawBackgroundEvent;	  // 画完桌面背景后被调用，参数：目标DC
	extern CD_API PreEvent<HDC&>                            g_postDrawIconEvent;	      // 画完图标后被调用，参数：目标DC

	// hook的事件
	extern CD_API PreEvent<CONST RECT*, HRGN, UINT>         g_fileListRedrawWindowEvent;  // comctl调用RedrawWindow，参数：lprcUpdate, hrgnUpdate, flags
	extern CD_API PreEvent<LPPAINTSTRUCT, HDC&>             g_fileListBeginPaintEvent;    // comctl调用BeginPaint，参数：lpPaint, 返回值
    extern CD_API PostEvent<LPPAINTSTRUCT>                  g_fileListEndPaintEvent;      // comctl调用EndPaint，参数：lpPaint
	
	// 窗口过程的事件
    extern CD_API PreEvent<UINT, WPARAM, LPARAM, LRESULT&>  g_fileListWndProcEvent;		  // 参数：message, wParam, lParam, 返回值
	extern CD_API PreEvent<UINT, WPARAM, LPARAM, LRESULT&>  g_parentWndProcEvent;		  // 参数：message, wParam, lParam, 返回值
	extern CD_API PreEvent<UINT, WPARAM, LPARAM, LRESULT&>  g_topWndProcEvent;		      // 参数：message, wParam, lParam, 返回值
}
