#include "stdafx.h"
#include "Decoder.h"


CDecoder::CDecoder(LPCWSTR fileName, HRESULT* phr) :
	CBaseVideoRenderer(CLSID_NULL, _T("Renderer"), NULL, &(*phr = NOERROR))
{
	AddRef(); // 防止多次析构

	*phr = m_graph.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER);
	*phr = m_graph.QueryInterface(&m_control);

	*phr = m_graph->AddFilter(this, L"Renderer");
	*phr = m_graph->AddSourceFilter(fileName, L"Source", &m_source);

	// 连接源和渲染器
	CComPtr<IEnumPins> enumPins;
	CComPtr<IPin> sourcePin;
	CBasePin* rendererPin = GetPin(0);
	PIN_DIRECTION pinDir;

	m_source->EnumPins(&enumPins);
	while (enumPins->Next(1, &sourcePin, NULL) == S_OK)
	{
		sourcePin->QueryDirection(&pinDir);
		if (pinDir == PINDIR_OUTPUT)
			break;
		sourcePin.Release();
	}
	enumPins.Release();

	*phr = m_graph->Connect(sourcePin, rendererPin);

	// 获取视频尺寸
	CMediaType mediaType;
	*phr = rendererPin->ConnectionMediaType(&mediaType);
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
}

void CDecoder::RunVideo()
{
	HRESULT hr = m_control->Run();
}

void CDecoder::PauseVideo()
{
	HRESULT hr = m_control->Pause();
}

void CDecoder::StopVideo()
{
	HRESULT hr = m_control->Stop();
}

void CDecoder::GetVideoSize(SIZE& size)
{
	size = m_videoSize;
}

void CDecoder::SetOnPresent(std::function<void(IMediaSample*)>&& onPresent)
{
	m_onPresent = std::move(onPresent);
}

// CBaseVideoRenderer

HRESULT CDecoder::CheckMediaType(const CMediaType * mediaType)
{
	if (mediaType->majortype == MEDIATYPE_Video && mediaType->subtype == MEDIASUBTYPE_RGB32)
		return S_OK;
	return VFW_E_INVALIDMEDIATYPE;
}

HRESULT CDecoder::DoRenderSample(IMediaSample *pMediaSample)
{
	if (!m_onPresent._Empty())
		m_onPresent(pMediaSample);
	return S_OK;
}
