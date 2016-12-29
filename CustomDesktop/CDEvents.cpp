#include "stdafx.h"
#include <CDEvents.h>
#include <vector>


namespace cd
{
#define DEF_VARIABLE(name) decltype(name) name

	CD_API DEF_VARIABLE(g_preUnloadEvent);

	CD_API DEF_VARIABLE(g_drawBackgroundEvent);
	CD_API DEF_VARIABLE(g_fileListBeginPaintEvent);
	CD_API DEF_VARIABLE(g_fileListEndPaintEvent);

	CD_API DEF_VARIABLE(g_fileListWndSizeEvent);
	CD_API DEF_VARIABLE(g_fileListWndProcEvent);
	CD_API DEF_VARIABLE(g_parentWndProcEvent);


	std::vector<EventBase*> g_events{
		&g_preUnloadEvent,

		&g_drawBackgroundEvent,
		&g_fileListBeginPaintEvent,
		&g_fileListEndPaintEvent,

		&g_fileListWndSizeEvent,
		&g_fileListWndProcEvent,
		&g_parentWndProcEvent
	};
}
