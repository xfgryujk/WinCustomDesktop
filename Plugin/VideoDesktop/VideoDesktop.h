#pragma once
#include "VideoPlayer.h"
#include <memory>
#include <atlimage.h>
#include <mutex>


class VideoDesktop final
{
public:
	VideoDesktop(HMODULE hModule);
	~VideoDesktop();

private:
	HMODULE m_module;

	const UINT WM_GRAPHNOTIFY;
	// 用多个player实现无缝跳转，如果用SeekVideo会有很长时间的停顿
	std::unique_ptr<VideoPlayer> m_players[2];
	VideoPlayer* m_curPlayer;
	int m_curPlayerIndex = 0;
	SIZE m_videoSize;

	CImage m_img;
	void* m_imgData = NULL;
	std::mutex m_imgDataLock;

	const UINT m_menuID;


	void InitPlayers();
	bool InitPlayer(std::unique_ptr<VideoPlayer>& player);

	bool OnPostDrawBackground(HDC& hdc);
	void OnPresent(IMediaSample* mediaSample);

	bool OnFileListWndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& res);

	bool OnAppendTrayMenu(HMENU menu);
	bool OnChooseMenuItem(UINT menuID);
};
