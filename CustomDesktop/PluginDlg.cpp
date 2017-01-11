// PluginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PluginDlg.h"
#include "PluginManager.h"
using namespace cd;


// CPluginDlg 对话框


IMPLEMENT_DYNAMIC(CPluginDlg, CModelessDlg<CPluginDlg>)

CPluginDlg::CPluginDlg(CWnd* pParent /*=NULL*/) :
	CModelessDlg<CPluginDlg>(CPluginDlg::IDD, pParent)
{
}

CPluginDlg::~CPluginDlg()
{
}

void CPluginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_pluginList);
}


BEGIN_MESSAGE_MAP(CPluginDlg, CDialog)
END_MESSAGE_MAP()


// CPluginDlg 消息处理程序

BOOL CPluginDlg::OnInitDialog()
{
	__super::OnInitDialog();

	m_pluginList.SetExtendedStyle(m_pluginList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
	int i = 0;
	m_pluginList.InsertColumn(i++, _T("启用"), LVCFMT_LEFT, 50);
	m_pluginList.InsertColumn(i++, _T("插件名"), LVCFMT_LEFT, 340);

	ShowPlugins();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CPluginDlg::ShowPlugins()
{
	m_pluginList.DeleteAllItems();
	const auto& plugins = PluginManager::GetInstance().GetPlugins();
	for (const auto& i : plugins)
	{
		int index = m_pluginList.GetItemCount();
		m_pluginList.InsertItem(index, _T(""));
		m_pluginList.SetItemText(index, 1, i.m_pureName.c_str());
	}
}
