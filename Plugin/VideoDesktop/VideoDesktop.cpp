#include "stdafx.h"
#include "VideoDesktop.h"
#include <CDEvents.h>
#include <CDAPI.h>
#include "VDConfig.h"


VideoDesktop::VideoDesktop(HMODULE hModule) : 
	m_module(hModule), 
	WM_GRAPHNOTIFY(cd::GetCustomMessageID())
{
	// 监听事件
	cd::g_drawBackgroundEvent.AddListener(std::bind(&VideoDesktop::OnDrawBackground, this, std::placeholders::_1, 
		std::placeholders::_2), m_module);
	cd::g_fileListWndProcEvent.AddListener(std::bind(&VideoDesktop::OnFileListWndProc, this, std::placeholders::_1,
		std::placeholders::_2, std::placeholders::_3), m_module);

	cd::ExecInMainThread([this]{
		// 初始化播放器
		for (auto& player : m_players)
		{
			if (!InitPlayer(player))
				return;
		}
		m_curPlayer = m_players[m_curPlayerIndex].get();

		m_curPlayer->GetVideoSize(m_videoSize);
		m_dc.Create(m_videoSize.cx, m_videoSize.cy, 32);

		m_curPlayer->RunVideo();
	});
}

bool VideoDesktop::InitPlayer(std::unique_ptr<VideoPlayer>& player)
{
	HRESULT hr;
	player = std::make_unique<VideoPlayer>(g_config.m_videoPath.c_str(), &hr);
	if (FAILED(hr))
	{
		player = nullptr;
		MessageBox(NULL, _T("加载视频失败！"), APPNAME, MB_ICONERROR);
		return false;
	}

	player->SetVolume(g_config.m_volume - 100);
	player->SetOnPresent(std::bind(&VideoDesktop::OnPresent, this, std::placeholders::_1));
	player->SetNotifyWindow(cd::GetFileListHwnd(), WM_GRAPHNOTIFY);
	return true;
}


bool VideoDesktop::OnDrawBackground(HDC& hdc, bool isInBeginPaint)
{
	if ((HDC)m_dc == NULL)
		return true;

	// 抗锯齿
	int oldMode = SetStretchBltMode(hdc, HALFTONE);

	SIZE size;
	cd::GetDesktopSize(size);
	m_dcLock.lock();
	StretchBlt(hdc, 0, 0, size.cx, size.cy, m_dc, 0, 0, m_videoSize.cx, m_videoSize.cy, SRCCOPY);
	m_dcLock.unlock();

	SetStretchBltMode(hdc, oldMode);
	return false;
}

void VideoDesktop::OnPresent(IMediaSample* mediaSample)
{
	BYTE* sampleBuf = NULL;
	if (FAILED(mediaSample->GetPointer(&sampleBuf)))
		return;

	if (mediaSample->GetActualDataLength() < m_videoSize.cx * m_videoSize.cy * 4)
		return;
	size_t size = min(mediaSample->GetActualDataLength(), m_videoSize.cx * m_videoSize.cy * 4);

	m_dcLock.lock();
	// RGB位图都是从下到上储存的
	memcpy(m_dc.GetPixelAddress(0, m_videoSize.cy - 1), sampleBuf, size);
	m_dcLock.unlock();

	cd::RedrawDesktop();
}


bool VideoDesktop::OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_GRAPHNOTIFY)
	{
		auto event = (IMediaEventEx*)lParam;
		long eventCode;
		LONG_PTR lParam1, lParam2;
		while (SUCCEEDED(event->GetEvent(&eventCode, &lParam1, &lParam2, 0)))
		{
			if (FAILED(event->FreeEventParams(eventCode, lParam1, lParam2)))
				break;
			// 播放完毕
			if (eventCode == EC_COMPLETE)
			{
				int prevPlayerIndex = m_curPlayerIndex;
				if (++m_curPlayerIndex >= _countof(m_players))
					m_curPlayerIndex = 0;
				m_curPlayer = m_players[m_curPlayerIndex].get();
				m_curPlayer->RunVideo();
				if (!InitPlayer(m_players[prevPlayerIndex]))
					m_curPlayer->StopVideo();
				break; // event已被释放
			}
		}
		return false;
	}
	return true;
}
