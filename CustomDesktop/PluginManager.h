#pragma once
#include "Singleton.h"
#include <string>
#include <vector>


namespace cd
{
	struct Plugin
	{
		bool m_enable;
		std::wstring m_path;          // 完整路径
		std::wstring m_sectionName;   // 配置文件中的节名
		HMODULE m_module;

		// 读取插件配置
		void Load(const std::wstring& path, const std::wstring& sectionName);
	};

	class PluginManager final : public Singleton<PluginManager>
	{
		DECL_SINGLETON(PluginManager);
	public:
		bool IsReady() { return true; }

		void Init();
		void Uninit();

		static std::wstring GetPluginListPath();
		const std::vector<Plugin>& GetPlugins() { return m_plugins; }

		// 加载插件DLL
		bool LoadPlugin(Plugin& plugin);
		//void LoadDir(const std::wstring& dir);
		// 读取ini文件中的所有插件设置并加载
		void LoadPluginList(const std::wstring& path);
		bool UnloadPlugin(int index);
		//bool UnloadPlugin(const std::wstring& pureName);
		bool UnloadAll();


	private:
		PluginManager();
		~PluginManager();


		std::vector<Plugin> m_plugins;


		bool OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam);
	};
}
