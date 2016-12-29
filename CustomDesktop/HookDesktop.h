#pragma once
#include "Singleton.h"
#include "IATHook.h"


namespace cd
{
	class HookDesktop final : public Singleton<HookDesktop>
	{
		DECL_SINGLETON(HookDesktop);
	public:
		bool IsReady() { return m_hasInit; }

		bool Init();
		bool Init(HWND fileListWnd);
		bool Uninit();

	private:
		bool m_hasInit = false;

		HookDesktop();
		~HookDesktop();


		WNDPROC m_oldFileListWndProc = NULL;
		WNDPROC m_oldParentWndProc = NULL;

		typedef HDC(WINAPI* BeginPaintType)(HWND hWnd, LPPAINTSTRUCT lpPaint);
		typedef BOOL(WINAPI* EndPaintType)(HWND hWnd, CONST PAINTSTRUCT *lpPaint);
		CIATHook<BeginPaintType> m_beginPaintHook;
		CIATHook<EndPaintType> m_endPaintHook;

		static LRESULT CALLBACK FileListWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK ParentWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		static HDC WINAPI MyBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint);
		static BOOL WINAPI MyEndPaint(HWND hWnd, CONST PAINTSTRUCT *lpPaint);

		LRESULT OnFileListWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT OnParentWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		HDC OnBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint);
		BOOL OnEndPaint(HWND hWnd, CONST PAINTSTRUCT *lpPaint);

		LRESULT OnDrawBackground(HDC hdc);
	};
}
