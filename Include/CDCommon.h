#pragma once

#ifdef CD_EXPORTS
#define CD_API __declspec(dllexport)
#else
#define CD_API __declspec(dllimport)
#pragma comment(lib, "CustomDesktop.lib")
#endif
