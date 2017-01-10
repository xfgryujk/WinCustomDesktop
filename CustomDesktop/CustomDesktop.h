#pragma once


namespace cd
{
	class CCustomDesktopApp final : public CWinApp
	{
	public:
		virtual BOOL InitInstance();
		virtual int ExitInstance();


	private:
		LPTOP_LEVEL_EXCEPTION_FILTER m_oldExceptionHandler = NULL;


		bool InitModules(HMODULE hModule);
		bool OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& res);

		static LONG WINAPI ExceptionHandler(_EXCEPTION_POINTERS* ExceptionInfo);
	};
}
