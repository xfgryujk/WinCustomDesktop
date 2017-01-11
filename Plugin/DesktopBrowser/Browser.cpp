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


Browser::Browser(HWND container, const RECT& pos, HRESULT& hr) :
	m_container(container),
	m_posRect(pos)
{
	hr = S_OK;
	if (FAILED(hr = OleInitialize(NULL))) return;

	if (FAILED(hr = StgCreateStorageEx(NULL, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT | STGM_CREATE, STGFMT_STORAGE,
		0, NULL, NULL, IID_IStorage, (void**)&m_storage))) return;
	if (FAILED(hr = OleCreate(CLSID_WebBrowser, IID_IOleObject, OLERENDER_DRAW, NULL, this, m_storage, (LPVOID*)&m_oleObject))) return;
	if (FAILED(hr = m_oleObject.QueryInterface(&m_oleInPlaceObject))) return;
	if (FAILED(hr = m_oleObject.QueryInterface(&m_webBrowser))) return;
	if (FAILED(hr = m_oleObject.QueryInterface(&m_viewObject))) return;

	if (FAILED(hr = m_oleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, this, 0, m_container, &m_posRect))) return;

	Navigate(L"about:blank");
}

Browser::~Browser()
{
	if (m_oleObject.p != NULL)
		m_oleObject->Close(OLECLOSE_NOSAVE);

	OleUninitialize();
}


void Browser::SetPos(const RECT& pos)
{
	m_posRect = pos;
	if (m_oleInPlaceObject.p != NULL)
		m_oleInPlaceObject->SetObjectRects(&m_posRect, &m_posRect);
}

//HWND Browser::GetBrowserHwnd()
//{
//	if (m_browserHwnd != NULL)
//		return m_browserHwnd;
//
//	HWND hwnd;
//	if (FAILED(m_oleInPlaceObject->GetWindow(&hwnd)))
//		return NULL;
//	EnumChildWindows(hwnd, [](HWND hwnd, LPARAM thiz)->BOOL{
//		TCHAR className[100];
//		GetClassName(hwnd, className, _countof(className));
//		if (_tcscmp(className, _T("Internet Explorer_Server")) == 0)
//		{
//			((Browser*)thiz)->m_browserHwnd = hwnd;
//			return FALSE;
//		}
//		return TRUE;
//	}, (LPARAM)this);
//
//	return m_browserHwnd;
//}

void Browser::Navigate(LPCWSTR url)
{
	if (m_webBrowser.p != NULL)
		m_webBrowser->Navigate2(&CComVariant(url), &CComVariant(navNoReadFromCache | navNoWriteToCache), NULL, NULL, NULL);
}

void Browser::Draw(HDC hdc, const RECT& dstRect)
{
	if (m_viewObject.p != NULL)
		m_viewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hdc, (LPCRECTL)&dstRect, NULL, NULL, 0);
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
	*ppvObject = NULL;
	if (riid == IID_IUnknown)
		*ppvObject = (IUnknown*)(IOleClientSite*)this;
	else if (riid == IID_IOleClientSite)
		*ppvObject = (IOleClientSite*)this;
	else if (riid == IID_IOleWindow)
		*ppvObject = (IOleWindow*)(IOleInPlaceSite*)this;
	else if (riid == IID_IOleInPlaceSite)
		*ppvObject = (IOleInPlaceSite*)this;
	else if (riid == IID_IOleInPlaceUIWindow)
		*ppvObject = (IOleInPlaceUIWindow*)this;
	else if (riid == IID_IOleInPlaceFrame)
		*ppvObject = (IOleInPlaceFrame*)this;
	return *ppvObject != NULL ? S_OK : E_NOINTERFACE;
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


// IOleInPlaceUIWindow

HRESULT STDMETHODCALLTYPE Browser::GetBorder(__RPC__out LPRECT lprectBorder)
{
	*lprectBorder = m_posRect;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Browser::RequestBorderSpace(__RPC__in_opt LPCBORDERWIDTHS pborderwidths)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Browser::SetBorderSpace(__RPC__in_opt LPCBORDERWIDTHS pborderwidths)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Browser::SetActiveObject(__RPC__in_opt IOleInPlaceActiveObject *pActiveObject, __RPC__in_opt_string LPCOLESTR pszObjName)
{
	return S_OK;
}


// IOleInPlaceFrame

HRESULT STDMETHODCALLTYPE Browser::InsertMenus(__RPC__in HMENU hmenuShared, __RPC__inout LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE Browser::SetMenu(__RPC__in HMENU hmenuShared, __RPC__in HOLEMENU holemenu, __RPC__in HWND hwndActiveObject)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE Browser::RemoveMenus(__RPC__in HMENU hmenuShared)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE Browser::SetStatusText(__RPC__in_opt LPCOLESTR pszStatusText)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE Browser::EnableModeless(BOOL fEnable)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE Browser::TranslateAccelerator(__RPC__in LPMSG lpmsg, WORD wID)
{
	return S_OK;
}
