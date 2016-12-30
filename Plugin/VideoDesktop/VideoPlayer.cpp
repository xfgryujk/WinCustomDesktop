#include "stdafx.h"
#include "VideoPlayer.h"


VideoPlayer::VideoPlayer(LPCWSTR fileName, HRESULT* phr) :
	CBaseVideoRenderer(CLSID_NULL, _T("Renderer"), NULL, &(*phr = NOERROR))
{
	AddRef(); // 防止多次析构

	if (FAILED(*phr = m_graph.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER))) return;
	if (FAILED(*phr = m_graph.QueryInterface(&m_control))) return;
	if (FAILED(*phr = m_graph.QueryInterface(&m_basicAudio))) return;
	if (FAILED(*phr = m_audioRenderer.CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER))
		&& FAILED(*phr = m_audioRenderer.CoCreateInstance(CLSID_AudioRender, NULL, CLSCTX_INPROC_SERVER))) return;

	if (FAILED(*phr = m_graph->AddSourceFilter(fileName, L"Source", &m_source))) return;
	if (FAILED(*phr = m_graph->AddFilter(this, L"Video Renderer"))) return;
	if (FAILED(*phr = m_graph->AddFilter(m_audioRenderer, L"Audio Renderer"))) return;

	// 连接源和渲染器
	{
		CComPtr<IEnumPins> enumPins;
		CComPtr<IPin> sourcePin;
		CComPtr<IFilterGraph2> graph2;
		if (FAILED(*phr = m_graph.QueryInterface(&graph2))) return;

		if (FAILED(*phr = m_source->EnumPins(&enumPins))) return;
		while (enumPins->Next(1, &sourcePin, NULL) == S_OK)
		{
			if (FAILED(*phr = graph2->RenderEx(sourcePin, AM_RENDEREX_RENDERTOEXISTINGRENDERERS, NULL)))
			{
				sourcePin.Release();
				return;
			}
			sourcePin.Release();
		}
	}

	// 获取视频尺寸
	CMediaType mediaType;
	if (FAILED(*phr = GetPin(0)->ConnectionMediaType(&mediaType))) return;
	if (mediaType.formattype == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER* info = (VIDEOINFOHEADER*)mediaType.pbFormat;
		m_videoSize.cx = info->bmiHeader.biWidth;
		m_videoSize.cy = info->bmiHeader.biHeight;
	}
	else
	{
		m_videoSize.cx = 800;
		m_videoSize.cy = 600;
	}

	*phr = S_OK;
}


void VideoPlayer::RunVideo()
{
	if (m_control.p != NULL)
		m_control->Run();
}

void VideoPlayer::PauseVideo()
{
	if (m_control.p != NULL)
		m_control->Pause();
}

void VideoPlayer::StopVideo()
{
	if (m_control.p != NULL)
		m_control->Stop();
}


void VideoPlayer::GetVideoSize(SIZE& size)
{
	size = m_videoSize;
}

int VideoPlayer::GetVolume()
{
	long volume = 0;
	if (SUCCEEDED(m_basicAudio->get_Volume(&volume)))
		return volume / 100;
	return 0;
}

void VideoPlayer::SetVolume(int volume)
{
	if (volume < -100)
		volume = -100;
	else if (volume > 0)
		volume = 0;
	m_basicAudio->put_Volume(volume * 100);
}


void VideoPlayer::SetOnPresent(std::function<void(IMediaSample*)> onPresent)
{
	m_onPresent = std::move(onPresent);
}


// CBaseVideoRenderer

HRESULT VideoPlayer::CheckMediaType(const CMediaType * mediaType)
{
	if (mediaType->majortype == MEDIATYPE_Video && mediaType->subtype == MEDIASUBTYPE_RGB32)
		return S_OK;
	return VFW_E_INVALIDMEDIATYPE;
}

HRESULT VideoPlayer::DoRenderSample(IMediaSample *pMediaSample)
{
	if (!m_onPresent._Empty())
		m_onPresent(pMediaSample);
	return S_OK;
}
