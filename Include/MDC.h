#pragma once
#include "CDCommon.h"


namespace cd
{
	class CD_API MDC final
	{
	private:
		HBITMAP m_oldBmp = NULL;
		HDC m_mdc = NULL;

	public:
		MDC() = default;
		MDC(int width, int height);
		~MDC();

		bool Create(int width, int height, WORD biBitCount = 32);
		void Release();
		operator HDC ();
	};
}
