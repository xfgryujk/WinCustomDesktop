#include "stdafx.h"
#include "InternalEvents.h"


namespace cd
{
#define DEF_VARIABLE(name) decltype(name) name

	DEF_VARIABLE(g_onDrawBackgroundEvent);
	DEF_VARIABLE(g_fileListWndSizeEvent);
	DEF_VARIABLE(g_fileListBeginPaintEvent);
	DEF_VARIABLE(g_fileListEndPaintEvent);
}
