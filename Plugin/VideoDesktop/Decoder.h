#pragma once
#include <streams.h>
#include <functional>


class CDecoder : private CBaseVideoRenderer
{
public:
	CDecoder(LPCWSTR fileName, HRESULT* phr);
	virtual ~CDecoder() = default;

	virtual void RunVideo();
	virtual void PauseVideo();
	virtual void StopVideo();

	virtual void GetVideoSize(SIZE& size);

	// 设置需要呈现时的回调函数
	virtual void SetOnPresent(std::function<void(IMediaSample*)> onPresent);

protected:
	CComPtr<IGraphBuilder> m_graph;
	CComPtr<IMediaControl> m_control;
	CComPtr<IBaseFilter> m_source;

	SIZE m_videoSize;
	// 需要呈现时被调用
	std::function<void(IMediaSample*)> m_onPresent;

	// CBaseVideoRenderer

private:

	HRESULT DoRenderSample(IMediaSample *pMediaSample);
	HRESULT CheckMediaType(const CMediaType *);
};
