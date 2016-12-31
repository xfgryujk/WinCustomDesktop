#pragma once
#include "Singleton.h"
#include "IATHook.h"
#include <vector>


namespace cd
{
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


		WNDPROC m_oldFileListWndProc = NULL;
		WNDPROC m_oldParentWndProc = NULL;

		typedef HDC(WINAPI* BeginPaintType)(HWND hWnd, LPPAINTSTRUCT lpPaint);
		typedef BOOL(WINAPI* EndPaintType)(HWND hWnd, LPPAINTSTRUCT lpPaint);
		std::vector<CIATHook<BeginPaintType> > m_beginPaintHooks;
		std::vector<CIATHook<EndPaintType> > m_endPaintHooks;

		static LRESULT CALLBACK FileListWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK ParentWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		static HDC WINAPI MyBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint);
		static BOOL WINAPI MyEndPaint(HWND hWnd, LPPAINTSTRUCT lpPaint);

		LRESULT OnFileListWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT OnParentWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		HDC OnBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint);
		BOOL OnEndPaint(HWND hWnd, LPPAINTSTRUCT lpPaint);


		bool m_isInBeginPaint = false;
	};
}
