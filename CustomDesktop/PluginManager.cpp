#include "stdafx.h"
#include "PluginManager.h"
#include <EventHelper.h>
#include "Global.h"
#include <CDEvents.h>
#include "HookDesktop.h"


namespace cd
{
	extern std::vector<EventBase*> g_events;

	// 准备卸载的消息
	static const UINT WM_PREUNLOAD = WM_APP + 99;


	bool PluginManager::LoadPlugin(LPCWSTR path)
	{
		Plugin plugin;
		plugin.m_path = path;
		plugin.m_module = LoadLibraryW(path);
		if (plugin.m_module == NULL)
			return false;

		m_plugins.push_back(std::move(plugin));
		return true;
	}

	void PluginManager::LoadDir(LPCWSTR dir)
	{
		WIN32_FIND_DATAW findFileData;
		HANDLE find = FindFirstFileW((std::wstring(dir) + L"\\*.dll").c_str(), &findFileData);
		if (find != INVALID_HANDLE_VALUE)
		{
			do
				LoadPlugin((std::wstring(dir) + L"\\" + findFileData.cFileName).c_str());
			while (FindNextFileW(find, &findFileData));
			FindClose(find);
		}
	}

	bool PluginManager::UnloadPlugin(int index)
	{
		auto& plugin = m_plugins[index];
		for (auto i : g_events)
			i->DeleteListenersOfModule(plugin.m_module);
		if (!FreeLibrary(plugin.m_module))
			return false;
		m_plugins.erase(m_plugins.begin() + index);
		return true;
	}

	bool PluginManager::UnloadAll()
	{
		bool res = true;
		for (int i = (int)m_plugins.size() - 1; i >= 0; --i)
			res = res && UnloadPlugin(i);
		return res;
	}


	PluginManager::PluginManager()
	{
		g_fileListWndProcEvent.AddListener(std::bind(&PluginManager::OnFileListWndProc, this, std::placeholders::_1, 
			std::placeholders::_2, std::placeholders::_3));

		LoadDir((g_global.m_cdDir + L"\\Plugin").c_str());
	}

	PluginManager::~PluginManager()
	{
		UnloadAll();
	}


	bool PluginManager::OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (message == WM_PREUNLOAD)
		{
			g_preUnloadEvent();
			// 卸载之前要释放所有插件否则卸载不掉
			UnloadAll();
			// 卸载hook防止崩溃
			HookDesktop::GetInstance().Uninit();
			return false;
		}
		return true;
	}
}
