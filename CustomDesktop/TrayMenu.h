#pragma once
#include "Singleton.h"
#include <shellapi.h>


namespace cd
{
	class TrayMenu final : public Singleton<TrayMenu>
	{
		DECL_SINGLETON(TrayMenu);
	public:
		bool IsReady() { return true; }

		bool Init();
		bool Uninit();

	private:
		TrayMenu();
		~TrayMenu();


		NOTIFYICONDATA m_trayData;
		UINT m_managePluginMenuID;
		UINT m_exitMenuID;


		void OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& res, bool& pass);
		void OnChooseMenuItem(UINT menuID, bool& pass);
	};
}
