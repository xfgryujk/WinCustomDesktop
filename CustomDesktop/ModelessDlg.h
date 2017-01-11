#pragma once
#include <memory>
#include <CDEvents.h>


template<class T>
class CModelessDlg : public CDialog
{
protected:
	CModelessDlg<T>(UINT nIDTemplate, CWnd* pParent = NULL) :
		CDialog(nIDTemplate, pParent)
	{
	}

	virtual ~CModelessDlg<T>() = default;

public:
	static void CreateInstance(UINT nIDTemplate = T::IDD, CWnd* pParentWnd = NULL)
	{
		if (s_instance != nullptr)
			return;
		
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		s_instance = std::make_unique<T>();
		if (s_instance->m_listenerID == -1)
		{
			// 用来在卸载DLL时销毁窗口
			s_instance->m_listenerID = cd::g_preUnloadEvent.AddListener([]{
				AFX_MANAGE_STATE(AfxGetStaticModuleState());
				if (s_instance != nullptr && s_instance->m_hWnd != NULL)
					s_instance->DestroyWindow();
				return true;
			});
		}
		s_instance->Create(nIDTemplate, pParentWnd);
	}

protected:
	virtual void PostNcDestroy()
	{
		CDialog::PostNcDestroy();

		if (m_listenerID != -1)
		{
			cd::g_preUnloadEvent.DeleteListener(m_listenerID);
			m_listenerID = -1;
		}
		s_instance = nullptr;
	}

	virtual void OnOK()
	{
		DestroyWindow();
	}

	virtual void OnCancel()
	{
		DestroyWindow();
	}


private:
	int m_listenerID = -1;
public:
	static std::unique_ptr<T> s_instance;
};

template<class T>
__declspec(selectany) std::unique_ptr<T> CModelessDlg<T>::s_instance;
