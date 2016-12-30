#include "stdafx.h"
#include "Decoder.h"


CDecoder::CDecoder(LPCWSTR fileName, HRESULT* phr) :
	CBaseVideoRenderer(CLSID_NULL, _T("Renderer"), NULL, &(*phr = NOERROR))
{
	AddRef(); // 防止多次析构

	if (FAILED(*phr = m_graph.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER))) return;
	if (FAILED(*phr = m_graph.QueryInterface(&m_control))) return;

	if (FAILED(*phr = m_graph->AddFilter(this, L"Renderer"))) return;
	if (FAILED(*phr = m_graph->AddSourceFilter(fileName, L"Source", &m_source))) return;

	// 连接源和渲染器
	CComPtr<IEnumPins> enumPins;
	CComPtr<IPin> sourcePin;
	CBasePin* rendererPin = GetPin(0);
	PIN_DIRECTION pinDir;

	if (FAILED(*phr = m_source->EnumPins(&enumPins))) return;
	while (enumPins->Next(1, &sourcePin, NULL) == S_OK)
	{
		if (SUCCEEDED(sourcePin->QueryDirection(&pinDir)) && pinDir == PINDIR_OUTPUT)
			break;
		sourcePin.Release();
	}
	enumPins.Release();

	if (FAILED(*phr = m_graph->Connect(sourcePin, rendererPin))) return;

	// 获取视频尺寸
	CMediaType mediaType;
	if (FAILED(*phr = rendererPin->ConnectionMediaType(&mediaType))) return;
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

void CDecoder::RunVideo()
{
	if (m_control.p != NULL)
		m_control->Run();
}

void CDecoder::PauseVideo()
{
	if (m_control.p != NULL)
		m_control->Pause();
}

void CDecoder::StopVideo()
{
	if (m_control.p != NULL)
		m_control->Stop();
}

void CDecoder::GetVideoSize(SIZE& size)
{
	size = m_videoSize;
}

void CDecoder::SetOnPresent(std::function<void(IMediaSample*)> onPresent)
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
