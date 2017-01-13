#include "stdafx.h"
#include "PluginManager.h"
#include "Global.h"
#include <CDEvents.h>
#include <CDAPI.h>
#include <sstream>
#include <fstream>
#include <memory>


namespace cd
{
	extern std::vector<EventBase*> g_externalEvents;


	void Plugin::Load(const std::wstring& path, const std::wstring& sectionName)
	{
		if (sectionName.empty())
		{
			m_enable = false;
			m_path.clear();
			m_sectionName.clear();
			m_module = NULL;
			return;
		}
		m_enable = GetPrivateProfileIntW(sectionName.c_str(), L"Enable", 1, path.c_str()) != 0;
		m_path = GetPluginDir() + sectionName + L".dll";
		m_sectionName = sectionName;
		m_module = NULL;
	}


	PluginManager::PluginManager()
	{
		Init();
	}

	PluginManager::~PluginManager()
	{
		Uninit();
	}

	void PluginManager::Init()
	{
		//LoadDir(GetPluginDir());
		LoadPluginList(GetPluginListPath());
	}

	void PluginManager::Uninit()
	{
		UnloadAll();
	}


	std::wstring PluginManager::GetPluginListPath()
	{
		return g_global.m_cdDir + L"\\Plugins.ini";
	}


	bool PluginManager::LoadPlugin(Plugin& plugin)
	{
		plugin.m_module = NULL;
		if (!plugin.m_enable)
			return true;
		if (plugin.m_path.empty())
			return false;
		_RPTFW1(_CRT_WARN, L"加载插件：%s\n", plugin.m_sectionName.c_str());

		plugin.m_module = LoadLibraryW(plugin.m_path.c_str());
		if (plugin.m_module == NULL)
		{
			_RPTFW1(_CRT_WARN, L"加载插件失败：%s\n", plugin.m_sectionName.c_str());
			std::wostringstream oss;
			oss << L"加载插件失败：" << plugin.m_sectionName;
			MessageBoxW(g_global.m_topWnd, oss.str().c_str(), L"CustomDesktop", MB_OK);
			return false;
		}

		m_plugins.push_back(std::move(plugin));
		return true;
	}

	/*void PluginManager::LoadDir(const std::wstring& dir)
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
	}*/

	void PluginManager::LoadPluginList(const std::wstring& path)
	{
		auto buffer = std::make_unique<WCHAR[]>(10240);
		DWORD size = GetPrivateProfileSectionNamesW(buffer.get(), 10240, path.c_str());
		std::wstring strName;
		for (LPCWSTR name = buffer.get(); name < buffer.get() + size; name += strName.size() + 1)
		{
			strName = name;

			Plugin plugin;
			plugin.Load(path, strName);
			LoadPlugin(plugin);
		}
	}

	bool PluginManager::UnloadPlugin(int index)
	{
		auto& plugin = m_plugins[index];
		if (plugin.m_module != NULL)
		{
			_RPTFW1(_CRT_WARN, L"卸载插件：%s\n", plugin.m_sectionName.c_str());

			for (auto i : g_externalEvents)
				i->DeleteListenersOfModule(plugin.m_module);
			if (!FreeLibrary(plugin.m_module))
				return false;
		}
		m_plugins.erase(m_plugins.begin() + index);
		return true;
	}

	/*bool PluginManager::UnloadPlugin(const std::wstring& pureName)
	{
		for (int i = 0; i < (int)m_plugins.size(); ++i)
		{
			if (_wcsicmp(m_plugins[i].m_pureName.c_str(), pureName.c_str()) == 0)
				return UnloadPlugin(i);
		}
		return false;
	}*/

	bool PluginManager::UnloadAll()
	{
		volatile bool res = true;
		for (int i = (int)m_plugins.size() - 1; i >= 0; --i)
			res = res && UnloadPlugin(i);
		return res;
	}
}
