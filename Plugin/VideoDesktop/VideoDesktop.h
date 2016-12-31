#pragma once
#include "VideoPlayer.h"
#include <memory>
#include <MDC.h>
#include <mutex>


class VideoDesktop final
{
public:
	VideoDesktop(HMODULE hModule);
	~VideoDesktop() = default;

private:
	HMODULE m_module;

	const UINT WM_GRAPHNOTIFY;
	// 实现无缝跳转
	std::unique_ptr<VideoPlayer> m_players[2];
	VideoPlayer* m_curPlayer;
	int m_curPlayerIndex = 0;
	SIZE m_videoSize;

	cd::MDC m_dc;
	std::mutex m_dcLock;


	bool InitPlayer(std::unique_ptr<VideoPlayer>& player);

	bool OnDrawBackground(HDC& hdc, bool isInBeginPaint);
	void OnPresent(IMediaSample* mediaSample);

	bool OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam);
};
