#pragma once
#include <ole2.h>
#include <ExDisp.h>


class BrowserInit final
{
private:
	static BrowserInit s_instance;

	BrowserInit();
	~BrowserInit() = default;
};

class Browser : protected IOleClientSite, protected IOleInPlaceSite, protected IOleInPlaceFrame
{
public:
	Browser(HWND container, const RECT& pos);
	virtual ~Browser();

	void SetPos(const RECT& pos);
	//HWND GetBrowserHwnd();
	void Navigate(LPCWSTR url);
	void Draw(HDC hdc, const RECT& dstRect);

protected:
	HWND m_container;
	RECT m_posRect;
	//HWND m_browserHwnd = NULL;

	CComPtr<IStorage> m_storage;
	CComPtr<IOleObject> m_oleObject;
	CComPtr<IOleInPlaceObject> m_oleInPlaceObject;
	CComPtr<IWebBrowser2> m_webBrowser;
	CComPtr<IViewObject> m_viewObject;

	// IUnknown
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject);

	// IOleClientSite
	virtual HRESULT STDMETHODCALLTYPE SaveObject(void);
	virtual HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, __RPC__deref_out_opt IMoniker **ppmk);
	virtual HRESULT STDMETHODCALLTYPE GetContainer(__RPC__deref_out_opt IOleContainer **ppContainer);
	virtual HRESULT STDMETHODCALLTYPE ShowObject(void);
	virtual HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL fShow);
	virtual HRESULT STDMETHODCALLTYPE RequestNewObjectLayout(void);

	// IOleWindow
	virtual HRESULT STDMETHODCALLTYPE GetWindow(__RPC__deref_out_opt HWND *phwnd);
	virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);

	// IOleInPlaceSite
	virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void);
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivate(void);
	virtual HRESULT STDMETHODCALLTYPE OnUIActivate(void);
	virtual HRESULT STDMETHODCALLTYPE GetWindowContext(__RPC__deref_out_opt IOleInPlaceFrame **ppFrame, 
		__RPC__deref_out_opt IOleInPlaceUIWindow **ppDoc, __RPC__out LPRECT lprcPosRect, __RPC__out LPRECT lprcClipRect,
		__RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo);
	virtual HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtant);
	virtual HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable);
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(void);
	virtual HRESULT STDMETHODCALLTYPE DiscardUndoState(void);
	virtual HRESULT STDMETHODCALLTYPE DeactivateAndUndo(void);
	virtual HRESULT STDMETHODCALLTYPE OnPosRectChange(__RPC__in LPCRECT lprcPosRect);

	// IOleInPlaceUIWindow
	HRESULT STDMETHODCALLTYPE GetBorder(__RPC__out LPRECT lprectBorder);
	HRESULT STDMETHODCALLTYPE RequestBorderSpace(__RPC__in_opt LPCBORDERWIDTHS pborderwidths);
	HRESULT STDMETHODCALLTYPE SetBorderSpace(__RPC__in_opt LPCBORDERWIDTHS pborderwidths);
	HRESULT STDMETHODCALLTYPE SetActiveObject(__RPC__in_opt IOleInPlaceActiveObject *pActiveObject, __RPC__in_opt_string LPCOLESTR pszObjName);

	// IOleInPlaceFrame
	HRESULT STDMETHODCALLTYPE InsertMenus(__RPC__in HMENU hmenuShared, __RPC__inout LPOLEMENUGROUPWIDTHS lpMenuWidths);
	HRESULT STDMETHODCALLTYPE SetMenu(__RPC__in HMENU hmenuShared, __RPC__in HOLEMENU holemenu, __RPC__in HWND hwndActiveObject);
	HRESULT STDMETHODCALLTYPE RemoveMenus(__RPC__in HMENU hmenuShared);
	HRESULT STDMETHODCALLTYPE SetStatusText(__RPC__in_opt LPCOLESTR pszStatusText);
	HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable);
	HRESULT STDMETHODCALLTYPE TranslateAccelerator(__RPC__in LPMSG lpmsg, WORD wID);
};
