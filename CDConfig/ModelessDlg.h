#pragma once
#include <memory>


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
		
		s_instance = std::make_unique<T>();
		s_instance->Create(nIDTemplate, pParentWnd);
	}

protected:
	virtual void PostNcDestroy()
	{
		CDialog::PostNcDestroy();

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


public:
	static std::unique_ptr<T> s_instance;
};

template<class T>
__declspec(selectany) std::unique_ptr<T> CModelessDlg<T>::s_instance;
