#include "stdafx.h"
#include <CDEvents.h>
#include <vector>


namespace cd
{
#define DEF_VARIABLE(name) decltype(name) name

	CD_API DEF_VARIABLE(g_preUnloadEvent);
	CD_API DEF_VARIABLE(g_desktopCoveredEvent);
	CD_API DEF_VARIABLE(g_desktopUncoveredEvent);
	CD_API DEF_VARIABLE(g_fileListWndSizeEvent);

	CD_API DEF_VARIABLE(g_appendTrayMenuEvent);
	CD_API DEF_VARIABLE(g_chooseMenuItemEvent);

	CD_API DEF_VARIABLE(g_preDrawBackgroundEvent);
	CD_API DEF_VARIABLE(g_postDrawBackgroundEvent);
	CD_API DEF_VARIABLE(g_postDrawIconEvent);

	CD_API DEF_VARIABLE(g_fileListRedrawWindowEvent);
	CD_API DEF_VARIABLE(g_fileListBeginPaintEvent);
	CD_API DEF_VARIABLE(g_fileListEndPaintEvent);

	CD_API DEF_VARIABLE(g_fileListWndProcEvent);
	CD_API DEF_VARIABLE(g_parentWndProcEvent);
	CD_API DEF_VARIABLE(g_topWndProcEvent);


	// 外部模块可注册的事件集合，用来自动卸载外部listener
	std::vector<EventBase*> g_externalEvents{
		&g_preUnloadEvent,
		&g_desktopCoveredEvent,
		&g_desktopUncoveredEvent,
		&g_fileListWndSizeEvent,

		&g_appendTrayMenuEvent,
		&g_chooseMenuItemEvent,

		&g_preDrawBackgroundEvent,
		&g_postDrawBackgroundEvent,
		&g_postDrawIconEvent,

		&g_fileListRedrawWindowEvent,
		&g_fileListBeginPaintEvent,
		&g_fileListEndPaintEvent,

		&g_fileListWndProcEvent,
		&g_parentWndProcEvent,
		&g_topWndProcEvent
	};
}
