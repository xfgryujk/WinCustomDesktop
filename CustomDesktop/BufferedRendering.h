#pragma once
#include "Singleton.h"
#include "MDC.h"


namespace cd
{
	class BufferedRendering final : public Singleton<BufferedRendering>
	{
		DECL_SINGLETON(BufferedRendering);
	public:
		bool IsReady() { return m_hasInit; }

		bool Init();
		bool Uninit();

	private:
		bool m_hasInit = false;

		BufferedRendering();
		~BufferedRendering();


		HDC m_originalDC = NULL;
		MDC m_bufferDC;

		bool OnFileListWndSize(int width, int height);
		bool OnFileListBeginPaint(LPPAINTSTRUCT lpPaint, HDC& res);
		bool OnFileListEndPaint(LPPAINTSTRUCT lpPaint);
	};
}
