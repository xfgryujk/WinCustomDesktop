#include "stdafx.h"
#include "VideoDesktop.h"
#include <CDEvents.h>
using namespace std::placeholders;
#include <CDAPI.h>
#include "Config.h"
#include <thread>
#include <shellapi.h>


VideoDesktop::VideoDesktop(HMODULE hModule) : 
	m_module(hModule), 
	WM_GRAPHNOTIFY(cd::GetFileListMsgID()),
	m_menuID(cd::GetMenuID())
{
	// 监听事件
	cd::g_desktopCoveredEvent.AddListener([this]{ if (m_curPlayer != NULL) m_curPlayer->PauseVideo(); }, m_module);
	cd::g_desktopUncoveredEvent.AddListener([this]{ if (m_curPlayer != NULL) m_curPlayer->RunVideo(); }, m_module);
	cd::g_preDrawBackgroundEvent.AddListener([this](HDC&, bool& pass){ pass = m_curPlayer == NULL || m_img.IsNull(); }, m_module);
	cd::g_postDrawBackgroundEvent.AddListener(std::bind(&VideoDesktop::OnPostDrawBackground, this, _1), m_module);
	cd::g_fileListWndProcEvent.AddListener(std::bind(&VideoDesktop::OnFileListWndProc, this, _1, _2, _3, _4, _5), m_module);
	cd::g_appendTrayMenuEvent.AddListener(std::bind(&VideoDesktop::OnAppendTrayMenu, this, _1), m_module);
	cd::g_chooseMenuItemEvent.AddListener(std::bind(&VideoDesktop::OnChooseMenuItem, this, _1, _2), m_module);

	cd::ExecInMainThread([this]{
		InitPlayers();
	});
}

VideoDesktop::~VideoDesktop()
{
	// 优先释放players！否则可能在析构时触发事件
	for (auto& i : m_players)
		i = nullptr;
}

void VideoDesktop::InitPlayers()
{
	// 初始化播放器
	for (auto& player : m_players)
	{
		if (!InitPlayer(player))
			return;
	}
	m_curPlayer = m_players[m_curPlayerIndex = 0].get();

	m_curPlayer->GetVideoSize(m_videoSize);
	if (!m_img.IsNull())
		m_img.Destroy();
	m_img.Create(m_videoSize.cx, m_videoSize.cy, 32/*, CImage::createAlphaChannel*/);
	// CImage偷偷改了BOTTOMUP位图的m_pBits...
	m_imgData = m_img.GetPixelAddress(0, m_videoSize.cy - 1);

	m_curPlayer->RunVideo();
}

bool VideoDesktop::InitPlayer(std::unique_ptr<VideoPlayer>& player)
{
	HRESULT hr;
	player = std::make_unique<VideoPlayer>(g_config.m_videoPath.c_str(), &hr);
	if (FAILED(hr))
	{
		player = nullptr;
		MessageBox(cd::GetTopHwnd(), _T("加载视频失败！"), APPNAME, MB_ICONERROR);
		return false;
	}

	player->SetVolume(g_config.m_volume - 100);
	player->SetOnPresent(std::bind(&VideoDesktop::OnPresent, this, _1));
	player->SetNotifyWindow(cd::GetFileListHwnd(), WM_GRAPHNOTIFY);
	return true;
}


void VideoDesktop::OnPostDrawBackground(HDC& hdc)
{
	if (m_img.IsNull())
		return;

	SIZE size;
	cd::GetDesktopSize(size);
	m_imgDataLock.lock();
	m_img.AlphaBlend(hdc, 0, 0, size.cx, size.cy, 0, 0, m_videoSize.cx, m_videoSize.cy);
	m_imgDataLock.unlock();
}

void VideoDesktop::OnPresent(IMediaSample* mediaSample)
{
	BYTE* sampleBuf = NULL;
	if (FAILED(mediaSample->GetPointer(&sampleBuf)))
		return;

	size_t sampleSize = mediaSample->GetActualDataLength();
	size_t imgSize = m_videoSize.cx * m_videoSize.cy * 4;
	if (mediaSample->GetActualDataLength() < m_videoSize.cx * m_videoSize.cy * 4)
		return;

	m_imgDataLock.lock();
	memcpy(m_imgData, sampleBuf, min(sampleSize, imgSize));
	// 保证alpha = 255
	/*for (BYTE* pPixel = (BYTE*)m_imgData; pPixel < (BYTE*)m_imgData + imgSize; pPixel += 4)
		pPixel[3] = 255;*/
	m_imgDataLock.unlock();

	cd::RedrawDesktop();
}


void VideoDesktop::OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& res, bool& pass)
{
	if (message == WM_GRAPHNOTIFY)
	{
		auto event = reinterpret_cast<IMediaEventEx*>(lParam);
		long eventCode;
		LONG_PTR lParam1, lParam2;
		while (SUCCEEDED(event->GetEvent(&eventCode, &lParam1, &lParam2, 0)))
		{
			if (FAILED(event->FreeEventParams(eventCode, lParam1, lParam2)))
				break;
			// 播放完毕
			if (eventCode == EC_COMPLETE)
			{
				const int prevPlayerIndex = m_curPlayerIndex;
				if (++m_curPlayerIndex >= _countof(m_players))
					m_curPlayerIndex = 0;
				m_curPlayer = m_players[m_curPlayerIndex].get();
				m_curPlayer->RunVideo();
				if (!InitPlayer(m_players[prevPlayerIndex]))
					m_curPlayer->StopVideo();
				break; // event已被释放
			}
		}
		res = 1;
		pass = false;
	}
}


void VideoDesktop::OnAppendTrayMenu(HMENU menu)
{
	AppendMenu(menu, MF_STRING, m_menuID, APPNAME);
}

void VideoDesktop::OnChooseMenuItem(UINT menuID, bool& pass)
{
	if (menuID != m_menuID)
		return;

	std::thread([this]{
		SHELLEXECUTEINFOW info = {};
		info.cbSize = sizeof(info);
		info.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC;
		info.lpVerb = L"open";
		info.lpFile = L"notepad.exe";
		std::wstring param = cd::GetPluginDir() + L"\\Data\\VideoDesktop.ini";
		info.lpParameters = param.c_str();
		info.nShow = SW_SHOWNORMAL;
		ShellExecuteExW(&info);

		WaitForSingleObject(info.hProcess, INFINITE);
		CloseHandle(info.hProcess);

		cd::ExecInMainThread([this]{
			Config newConfig;
			if (newConfig.m_volume != g_config.m_volume)
			{
				for (auto& i : m_players)
				{
					if (i != nullptr)
						i->SetVolume(newConfig.m_volume - 100);
				}
			}
			auto oldVideoPath = g_config.m_videoPath;
			g_config = newConfig;
			if (newConfig.m_videoPath != oldVideoPath)
				InitPlayers();
		});
	}).detach();
	pass = false;
}
