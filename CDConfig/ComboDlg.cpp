// ComboDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CDConfig.h"
#include "ComboDlg.h"


// CComboDlg 对话框

CComboDlg::CComboDlg(const std::vector<CString>& list, CWnd* pParent /*=NULL*/) : 
	CDialog(CComboDlg::IDD, pParent),
	m_list(list)
{
}

void CComboDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combo);
}

BEGIN_MESSAGE_MAP(CComboDlg, CDialog)
END_MESSAGE_MAP()


// CComboDlg 消息处理程序

BOOL CComboDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	for (const auto& i : m_list)
		m_combo.AddString(i);
	if (m_combo.GetCount() > 0)
		m_combo.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CComboDlg::OnOK()
{
	m_sel = m_combo.GetCurSel();

	CDialog::OnOK();
}
