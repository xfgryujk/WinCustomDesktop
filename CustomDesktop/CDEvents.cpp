#include "stdafx.h"
#include <CDEvents.h>


namespace cd
{
#define DEF_VARIABLE(name) decltype(name) name

	CD_API DEF_VARIABLE(g_drawBackgroundEvent);
	CD_API DEF_VARIABLE(g_fileListBeginPaintEvent);
	CD_API DEF_VARIABLE(g_fileListEndPaintEvent);
	
	CD_API DEF_VARIABLE(g_fileListWndSizeEvent);
	CD_API DEF_VARIABLE(g_fileListWndProcEvent);
	CD_API DEF_VARIABLE(g_parentWndProcEvent);
}
