#pragma once
#include "VideoPlayer.h"
#include <memory>
#include <atlimage.h>
#include <mutex>


class VideoDesktop final
{
public:
	VideoDesktop(HMODULE hModule);

private:
	HMODULE m_module;

	const UINT WM_GRAPHNOTIFY;
	// 实现无缝跳转
	std::unique_ptr<VideoPlayer> m_players[2];
	VideoPlayer* m_curPlayer;
	int m_curPlayerIndex = 0;
	SIZE m_videoSize;

	CImage m_img;
	void* m_imgData = NULL;
	std::mutex m_imgDataLock;


	bool InitPlayer(std::unique_ptr<VideoPlayer>& player);

	bool OnDrawBackground(HDC& hdc, bool isInBeginPaint);
	void OnPresent(IMediaSample* mediaSample);

	bool OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam);
};
