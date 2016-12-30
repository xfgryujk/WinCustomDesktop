#include "stdafx.h"
#include "VideoDesktop.h"
#include <CDEvents.h>
#include <CDAPI.h>
#include "VDConfig.h"


VideoDesktop::VideoDesktop(HMODULE hModule) : 
	m_module(hModule)
{
	// 监听事件
	cd::g_drawBackgroundEvent.AddListener(std::bind(&VideoDesktop::OnDrawBackground, this, std::placeholders::_1), m_module);

	cd::ExecInMainThread([this]{
		// 初始化解码器
		HRESULT hr;
		m_decoder = std::make_unique<CDecoder>(g_config.m_videoPath.c_str(), &hr);
		if (FAILED(hr))
		{
			MessageBox(NULL, _T("加载视频失败！"), APPNAME, MB_ICONERROR);
			return;
		}

		m_decoder->SetOnPresent(std::bind(&VideoDesktop::OnPresent, this, std::placeholders::_1));

		m_decoder->GetVideoSize(m_videoSize);
		m_dc.Create(m_videoSize.cx, m_videoSize.cy, 32);

		m_decoder->RunVideo();
	});
}

VideoDesktop::~VideoDesktop()
{
	if (m_decoder != nullptr)
		m_decoder->StopVideo();
}


bool VideoDesktop::OnDrawBackground(HDC hdc)
{
	// 抗锯齿
	int oldMode = SetStretchBltMode(hdc, HALFTONE);

	SIZE size;
	cd::GetWndSize(size);
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

	// 假设每行之间没有多余的字节
	//ASSERT(mediaSample->GetActualDataLength() == m_videoSize.cx * m_videoSize.cy * 4);
	m_dcLock.lock();
	// RGB位图都是从下到上储存的
	memcpy(m_dc.GetPixelAddress(0, m_videoSize.cy - 1), sampleBuf, m_videoSize.cx * m_videoSize.cy * 4);
	m_dcLock.unlock();

	InvalidateRect(cd::GetFileListHwnd(), NULL, FALSE);
}
