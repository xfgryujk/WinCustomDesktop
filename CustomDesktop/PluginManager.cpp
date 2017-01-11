#include "stdafx.h"
#include "PluginManager.h"
#include "Global.h"
#include <CDEvents.h>
#include <CDAPI.h>
#include <sstream>
#include <fstream>


namespace cd
{
	extern std::vector<EventBase*> g_externalEvents;


	PluginManager::PluginManager()
	{
		//LoadDir(GetPluginDir());
		LoadPluginList(GetPluginListPath());
	}

	PluginManager::~PluginManager()
	{
		UnloadAll();
	}


	std::wstring PluginManager::GetPluginListPath()
	{
		return g_global.m_cdDir + L"\\Plugins.txt";
	}


	bool PluginManager::LoadPlugin(const std::wstring& path)
	{
		_RPTFW1(_CRT_WARN, L"加载插件：%s\n", path.c_str());

		Plugin plugin;
		plugin.m_path = path;

		size_t pos1 = path.rfind(L'\\');
		if (pos1 == std::wstring::npos)
			pos1 = path.rfind(L'/');
		size_t pos2 = path.rfind(L'.');
		if (pos2 == std::wstring::npos)
			pos2 = path.size();
		if (pos1 == std::wstring::npos)
			plugin.m_pureName = path;
		else
			plugin.m_pureName = path.substr(pos1 + 1, pos2 - pos1 - 1);

		plugin.m_module = LoadLibraryW(path.c_str());

		if (plugin.m_module == NULL)
		{
			_RPTFW1(_CRT_WARN, L"加载插件失败：%s\n", path.c_str());
			std::wostringstream oss;
			oss << L"加载插件失败" << path;
			MessageBoxW(g_global.m_topWnd, oss.str().c_str(), L"CustomDesktop", MB_OK);
			return false;
		}

		m_plugins.push_back(std::move(plugin));
		return true;
	}

	void PluginManager::LoadDir(const std::wstring& dir)
	{
		WIN32_FIND_DATAW findFileData;
		HANDLE find = FindFirstFileW((dir + L"\\*.dll").c_str(), &findFileData);
		if (find != INVALID_HANDLE_VALUE)
		{
			do
				LoadPlugin((dir + L"\\" + findFileData.cFileName).c_str());
			while (FindNextFileW(find, &findFileData));
			FindClose(find);
		}
	}

	void PluginManager::LoadPluginList(const std::wstring& path)
	{
		std::wifstream listStream(path);
		if (!listStream.is_open())
			return;

		std::wstring pluginDir = GetPluginDir();
		std::wstring pluginName;
		while (std::getline(listStream, pluginName))
			LoadPlugin(pluginDir + pluginName + L".dll");
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

	bool PluginManager::UnloadPlugin(const std::wstring& pureName)
	{
		for (int i = 0; i < (int)m_plugins.size(); ++i)
		{
			if (_wcsicmp(m_plugins[i].m_pureName.c_str(), pureName.c_str()) == 0)
				return UnloadPlugin(i);
		}
		return false;
	}

	bool PluginManager::UnloadAll()
	{
		bool res = true;
		for (int i = (int)m_plugins.size() - 1; i >= 0; --i)
			res = res && UnloadPlugin(i);
		return res;
	}
}
