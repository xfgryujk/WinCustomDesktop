#pragma once
#include "resource.h"
#include <vector>


// CComboDlg 对话框

class CComboDlg : public CDialog
{
public:
	CComboDlg(const std::vector<CString>& list, CWnd* pParent = NULL);
	virtual ~CComboDlg() = default;

// 对话框数据
	enum { IDD = IDD_COMBO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnOK();


	const std::vector<CString>& m_list;

public:
	CComboBox m_combo;

	int m_sel = -1;
};
