#include "stdafx.h"
#include "PluginManager.h"
#include <EventHelper.h>
#include "Global.h"
#include <CDEvents.h>
#include <CDAPI.h>


namespace cd
{
	extern std::vector<EventBase*> g_events;


	bool PluginManager::LoadPlugin(LPCWSTR path)
	{
		_RPTFW1(_CRT_WARN, L"加载MOD：%s\n", path);

		Plugin plugin;
		plugin.m_path = path;
		plugin.m_module = LoadLibraryW(path);
		if (plugin.m_module == NULL)
		{
			_RPTFW1(_CRT_ERROR, L"加载MOD失败：%s\n", path);
			return false;
		}

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
		_RPTFW1(_CRT_WARN, L"卸载MOD：%s\n", plugin.m_path.c_str());

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
		LoadDir(GetPluginDir().c_str());
	}

	PluginManager::~PluginManager()
	{
		UnloadAll();
	}
}
