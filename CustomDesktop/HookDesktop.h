#pragma once
#include "Singleton.h"
#include "IATHook.h"
#include <vector>
#include <CDEvents.h>


namespace cd
{
	// 各种hook，不实现逻辑
	class HookDesktop final : public Singleton<HookDesktop>
	{
		DECL_SINGLETON(HookDesktop);
	public:
		bool IsReady() { return m_hasInit; }

		bool Init();
		bool Uninit();

	private:
		bool m_hasInit = false;

		HookDesktop();
		~HookDesktop();


		typedef BOOL(WINAPI* RedrawWindowType)(HWND hWnd, CONST RECT *lprcUpdate, HRGN hrgnUpdate, UINT flags);
		typedef HDC(WINAPI* BeginPaintType)(HWND hWnd, LPPAINTSTRUCT lpPaint);
		typedef BOOL(WINAPI* EndPaintType)(HWND hWnd, LPPAINTSTRUCT lpPaint);
		std::vector<IATHook<RedrawWindowType> > m_redrawWindowHooks;
		std::vector<IATHook<BeginPaintType> > m_beginPaintHooks;
		std::vector<IATHook<EndPaintType> > m_endPaintHooks;

		static LRESULT CALLBACK FileListWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK ParentWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK TopWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		static BOOL WINAPI MyRedrawWindow(HWND hWnd, CONST RECT *lprcUpdate, HRGN hrgnUpdate, UINT flags);
		static HDC WINAPI MyBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint);
		static BOOL WINAPI MyEndPaint(HWND hWnd, LPPAINTSTRUCT lpPaint);
	};
}
