#include "stdafx.h"
#include "BufferedDesktop.h"


CBufferedDesktop::~CBufferedDesktop()
{
	Uninit();
}


BOOL CBufferedDesktop::Init()
{
	BOOL res = CCustomDesktop::Init();
	if (!res)
		return res;

	RECT rect;
	if (!GetClientRect(m_fileListWnd, &rect))
		rect = {};
	res = m_bufferDC.Create(rect.right - rect.left, rect.bottom - rect.top, 24);
	if (!res)
	{
		Uninit();
		return FALSE;
	}

	// 子类化
	m_oldWndProc = (WNDPROC)SetWindowLongPtr(m_fileListWnd, GWLP_WNDPROC, (ULONG_PTR)FileListWndProc);
	if (m_oldWndProc == NULL)
	{
		Uninit();
		return FALSE;
	}

	return TRUE;
}

void CBufferedDesktop::Uninit()
{
	if (IsWindow(m_fileListWnd) && m_oldWndProc != NULL)
		SetWindowLongPtr(m_fileListWnd, GWLP_WNDPROC, (ULONG_PTR)m_oldWndProc);

	CCustomDesktop::Uninit();
}


HDC CBufferedDesktop::OnBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint)
{
	HDC res = CCustomDesktop::OnBeginPaint(hWnd, lpPaint);
	if (res != NULL)
	{
		m_originalDC = res;
		res = lpPaint->hdc = m_bufferDC;
	}
	return res;
}

BOOL CBufferedDesktop::OnEndPaint(HWND hWnd, CONST PAINTSTRUCT *lpPaint)
{
	if (lpPaint->hdc != NULL)
	{
		const_cast<PAINTSTRUCT*>(lpPaint)->hdc = m_originalDC;

		BitBlt(lpPaint->hdc, lpPaint->rcPaint.left, lpPaint->rcPaint.top, lpPaint->rcPaint.right - lpPaint->rcPaint.left,
			lpPaint->rcPaint.bottom - lpPaint->rcPaint.top, m_bufferDC, lpPaint->rcPaint.left, lpPaint->rcPaint.top, SRCCOPY);
	}
	return CCustomDesktop::OnEndPaint(hWnd, lpPaint);
}


LRESULT CALLBACK CBufferedDesktop::FileListWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (s_instance == NULL)
		return 0;
	CBufferedDesktop* thiz = (CBufferedDesktop*)s_instance;

	return thiz->OnFileListWndProc(hwnd, message, wParam, lParam);
}

LRESULT CBufferedDesktop::OnFileListWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		m_bufferDC.Create(LOWORD(lParam), HIWORD(lParam), 24);
		break;
	}

	return CallWindowProc(m_oldWndProc, hwnd, message, wParam, lParam);
}
