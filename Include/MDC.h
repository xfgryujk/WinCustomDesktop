#pragma once
#include "CDCommon.h"


namespace cd
{
	class CD_API MDC final
	{
	private:
		HBITMAP m_oldBmp = NULL;
		HDC m_mdc = NULL;

		UINT m_width = 0, m_height = 0;
		UINT m_BPP = 0;
		void* m_pBits = NULL;

	public:
		MDC() = default;
		MDC(int width, int height);
		~MDC();

		bool Create(int width, int height, WORD biBitCount = 32);
		void Release();
		operator HDC ();
		void* GetPixelAddress(UINT x, UINT y);
	};
}
