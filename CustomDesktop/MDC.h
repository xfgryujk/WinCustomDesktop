#pragma once


class MDC
{
public:
	MDC();
	MDC(int width, int height);
	~MDC();

	HDC getDC();

private:
	HBITMAP oldBmp;
	HDC mdc;
};
