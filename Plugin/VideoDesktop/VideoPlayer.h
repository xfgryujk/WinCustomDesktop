#pragma once
#include <streams.h>
#include <functional>


class VideoPlayer : private CBaseVideoRenderer
{
public:
	VideoPlayer(LPCWSTR fileName, HRESULT* phr);
	virtual ~VideoPlayer();

	virtual void RunVideo();
	virtual void PauseVideo();
	virtual void StopVideo();
	virtual void SeekVideo(LONGLONG pos);

	virtual void GetVideoSize(SIZE& size);
	virtual int GetVolume(); // -100~0，分贝
	virtual void SetVolume(int volume); // -100~0，分贝

	// 设置需要呈现时的回调函数
	virtual void SetOnPresent(std::function<void(IMediaSample*)> onPresent);
	// 设置接收事件的窗口和消息，lParam是IMediaEventEx*
	virtual void SetNotifyWindow(HWND hwnd, UINT messageID);

protected:
	CComPtr<IGraphBuilder> m_graph;
	CComPtr<IMediaControl> m_control;
	CComPtr<IMediaSeeking> m_seeking;
	CComPtr<IMediaEventEx> m_event;
	CComPtr<IBasicAudio> m_basicAudio;

	CComPtr<IBaseFilter> m_source;
	CComPtr<IBaseFilter> m_audioRenderer;

	SIZE m_videoSize;
	// 需要呈现时被调用
	std::function<void(IMediaSample*)> m_onPresent;


	// CBaseVideoRenderer
private:
	HRESULT CheckMediaType(const CMediaType *);
	HRESULT DoRenderSample(IMediaSample *pMediaSample);
};
