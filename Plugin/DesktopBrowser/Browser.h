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

class Browser : protected IOleClientSite, protected IOleInPlaceSite
{
public:
	Browser(HWND container, SIZE& size);
	virtual ~Browser();

	void Navigate(LPCWSTR url);
	void Draw(HDC hdc, LPCRECT dstRect);

protected:
	HWND m_container;
	RECT m_posRect;

	CComPtr<IStorage> m_storage;
	CComPtr<IOleObject> m_oleObject;
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
};
