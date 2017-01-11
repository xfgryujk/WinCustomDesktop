#pragma once
#include "Singleton.h"
#include <string>
#include <vector>


namespace cd
{
	struct Plugin
	{
		std::wstring m_path;       // 完整路径
		std::wstring m_pureName;   // 不带扩展名的文件名
		HMODULE m_module;
	};

	class PluginManager final : public Singleton<PluginManager>
	{
		DECL_SINGLETON(PluginManager);
	public:
		bool IsReady() { return true; }

		static std::wstring GetPluginListPath();
		const std::vector<Plugin>& GetPlugins() { return m_plugins; }

		bool LoadPlugin(const std::wstring& path);
		void LoadDir(const std::wstring& dir);
		void LoadPluginList(const std::wstring& path);
		bool UnloadPlugin(int index);
		bool UnloadPlugin(const std::wstring& pureName);
		bool UnloadAll();


	private:
		PluginManager();
		~PluginManager();


		std::vector<Plugin> m_plugins;


		bool OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam);
	};
}
