#include "stdafx.h"
#include "PluginManager.h"
#include "Global.h"
#include <CDEvents.h>
#include <CDAPI.h>
#include <sstream>


namespace cd
{
	extern std::vector<EventBase*> g_externalEvents;


	bool PluginManager::LoadPlugin(LPCWSTR path)
	{
		_RPTFW1(_CRT_WARN, L"加载插件：%s\n", path);

		Plugin plugin;
		plugin.m_path = path;
		plugin.m_module = LoadLibraryW(path);
		if (plugin.m_module == NULL)
		{
			_RPTFW1(_CRT_WARN, L"加载插件失败：%s\n", path);
			std::wostringstream oss;
			oss << L"加载插件失败" << path;
			MessageBoxW(g_global.m_topWnd, oss.str().c_str(), L"CustomDesktop", MB_OK);
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
		_RPTFW1(_CRT_WARN, L"卸载插件：%s\n", plugin.m_path.c_str());

		for (auto i : g_externalEvents)
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
