#include "stdafx.h"
#include "Browser.h"
#include <memory>


BrowserInit BrowserInit::s_instance;

BrowserInit::BrowserInit()
{
	// 取IE版本
	DWORD versionInfoSize = GetFileVersionInfoSize(_T("mshtml.dll"), NULL);
	if (versionInfoSize == 0) return;
	auto versionInfo = std::make_unique<BYTE[]>(versionInfoSize);
	if (!GetFileVersionInfo(_T("mshtml.dll"), 0, versionInfoSize, versionInfo.get())) return;
	VS_FIXEDFILEINFO* fixedFileInfo = NULL;
	UINT fixedFileInfoSize = 0;
	if (!VerQueryValue(versionInfo.get(), _T("\\"), (LPVOID*)&fixedFileInfo, &fixedFileInfoSize)) return;
	DWORD ieVersion = HIWORD(fixedFileInfo->dwProductVersionMS);
	if (ieVersion < 8) return;

	// https://msdn.microsoft.com/en-us/library/ee330730(v=vs.85).aspx
	DWORD browserEmulation = 0;
	switch (ieVersion)
	{
	default: case 11: browserEmulation = 11000; break;
	case 10: browserEmulation = 10000; break;
	case 9: browserEmulation = 9000; break;
	case 8: browserEmulation = 8000; break;
	}

	// 设置FEATURE_BROWSER_EMULATION
	HKEY key;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION"),
		0, NULL, REG_OPTION_VOLATILE, KEY_WRITE, NULL, &key, NULL) != ERROR_SUCCESS) return;
	RegSetValueEx(key, _T("explorer.exe"), 0, REG_DWORD, (BYTE*)&browserEmulation, sizeof(browserEmulation));
	RegCloseKey(key);
}


Browser::Browser(HWND container, SIZE& size) :
	m_container(container)
{
	if (FAILED(OleInitialize(NULL))) return;

	if (FAILED(StgCreateStorageEx(NULL, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT | STGM_CREATE, STGFMT_STORAGE,
		0, NULL, NULL, IID_IStorage, (void**)&m_storage))) return;
	if (FAILED(OleCreate(CLSID_WebBrowser, IID_IOleObject, OLERENDER_DRAW, NULL, this, m_storage, (LPVOID*)&m_oleObject))) return;

	if (FAILED(m_oleObject.QueryInterface(&m_webBrowser))) return;
	if (FAILED(m_oleObject.QueryInterface(&m_viewObject))) return;

	// 隐藏激活
	m_posRect = { -32000, -32000, -32000 + size.cx, -32000 + size.cy };
	if (FAILED(m_oleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, this, 0, m_container, &m_posRect))) return;
	if (FAILED(m_oleObject->DoVerb(OLEIVERB_HIDE, NULL, this, 0, m_container, &m_posRect))) return;
}

Browser::~Browser()
{
	OleUninitialize();
}


void Browser::Navigate(LPCWSTR url)
{
	if (m_webBrowser.p != NULL)
		m_webBrowser->Navigate2(&CComVariant(url), &CComVariant(navNoReadFromCache | navNoWriteToCache), NULL, NULL, NULL);
}

void Browser::Draw(HDC hdc, LPCRECT dstRect)
{
	if (m_viewObject.p != NULL)
		m_viewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hdc, (LPCRECTL)dstRect, NULL, NULL, 0);
}


// IUnknown

ULONG STDMETHODCALLTYPE Browser::AddRef(void)
{
	return 0;
}

ULONG STDMETHODCALLTYPE Browser::Release(void)
{
	return 0;
}

HRESULT STDMETHODCALLTYPE Browser::QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)
{
	if (riid == IID_IUnknown)
	{
		*ppvObject = (IUnknown*)(IOleClientSite*)this;
		return S_OK;
	}
	if (riid == IID_IOleClientSite)
	{
		*ppvObject = (IOleClientSite*)this;
		return S_OK;
	}
	if (riid == IID_IOleWindow)
	{
		*ppvObject = (IOleWindow*)this;
		return S_OK;
	}
	if (riid == IID_IOleInPlaceSite)
	{
		*ppvObject = (IOleInPlaceSite*)this;
		return S_OK;
	}
	return E_NOINTERFACE;
}


// IOleClientSite

HRESULT STDMETHODCALLTYPE Browser::SaveObject(void)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE Browser::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, __RPC__deref_out_opt IMoniker **ppmk)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE Browser::GetContainer(__RPC__deref_out_opt IOleContainer **ppContainer)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE Browser::ShowObject(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Browser::OnShowWindow(BOOL fShow)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Browser::RequestNewObjectLayout(void)
{
	return S_OK;
}


// IOleWindow

HRESULT STDMETHODCALLTYPE Browser::GetWindow(__RPC__deref_out_opt HWND *phwnd)
{
	*phwnd = m_container;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Browser::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}


// IOleInPlaceSite

HRESULT STDMETHODCALLTYPE Browser::CanInPlaceActivate(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Browser::OnInPlaceActivate(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Browser::OnUIActivate(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Browser::GetWindowContext(__RPC__deref_out_opt IOleInPlaceFrame **ppFrame,
	__RPC__deref_out_opt IOleInPlaceUIWindow **ppDoc, __RPC__out LPRECT lprcPosRect, __RPC__out LPRECT lprcClipRect,
	__RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	*ppFrame = (IOleInPlaceFrame*)this;
	*ppDoc = (IOleInPlaceUIWindow*)this;
	*lprcPosRect = m_posRect;
	*lprcClipRect = m_posRect;
	lpFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
	lpFrameInfo->fMDIApp = FALSE;
	lpFrameInfo->hwndFrame = m_container;
	lpFrameInfo->haccel = NULL;
	lpFrameInfo->cAccelEntries = 0;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Browser::Scroll(SIZE scrollExtant)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE Browser::OnUIDeactivate(BOOL fUndoable)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Browser::OnInPlaceDeactivate(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Browser::DiscardUndoState(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Browser::DeactivateAndUndo(void)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Browser::OnPosRectChange(__RPC__in LPCRECT lprcPosRect)
{
	return S_OK;
}

