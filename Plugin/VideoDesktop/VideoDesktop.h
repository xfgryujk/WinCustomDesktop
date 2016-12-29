#pragma once
#include "Decoder.h"
#include <memory>
#include <MDC.h>
#include <mutex>


class VideoDesktop final
{
public:
	VideoDesktop(HMODULE hModule);
	~VideoDesktop();

private:
	HMODULE m_module;

	std::unique_ptr<CDecoder> m_decoder;
	SIZE m_videoSize;

	cd::MDC m_dc;
	std::mutex m_dcLock;


	bool OnDrawBackground(HDC hdc);
	void OnPresent(IMediaSample* mediaSample);
};
