#pragma once
#include "Singleton.h"
#include <string>
#include <vector>


namespace cd
{
	class PluginManager final : public Singleton<PluginManager>
	{
		DECL_SINGLETON(PluginManager);
	public:
		bool IsReady() { return true; }

		bool LoadPlugin(LPCWSTR path);
		void LoadDir(LPCWSTR dir);
		bool UnloadPlugin(int index);
		bool UnloadAll();


	private:
		PluginManager();
		~PluginManager();


		struct Plugin
		{
			std::wstring m_path;
			HMODULE m_module;
		};

		std::vector<Plugin> m_plugins;


		bool OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam);
	};
}
