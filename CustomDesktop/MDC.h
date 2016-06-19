#pragma once


class MDC
{
public:
	MDC() = default;
	MDC(int width, int height);
	~MDC();

	BOOL Create(int width, int height);
	void Release();
	operator HDC ();

private:
	HBITMAP m_oldBmp = NULL;
	HDC m_mdc = NULL;
};
