// CDConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CDConfigDlg.h"
#include <vector>


// CCDConfigDlg 对话框


CCDConfigDlg::CCDConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCDConfigDlg::IDD, pParent)
{
}

void CCDConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_pluginList);
}

BEGIN_MESSAGE_MAP(CCDConfigDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON6, &CCDConfigDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON5, &CCDConfigDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON4, &CCDConfigDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON3, &CCDConfigDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &CCDConfigDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CCDConfigDlg 消息处理程序

BOOL CCDConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pluginList.SetExtendedStyle(m_pluginList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
	int i = 0;
	m_pluginList.InsertColumn(i++, _T("启用"), LVCFMT_LEFT, 50);
	m_pluginList.InsertColumn(i++, _T("插件名"), LVCFMT_LEFT, 340);

	ShowPlugins();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCDConfigDlg::ShowPlugins()
{
	m_pluginList.DeleteAllItems();
	/*const auto& plugins = PluginManager::GetInstance().GetPlugins();
	for (const auto& i : plugins)
	{
		int index = m_pluginList.GetItemCount();
		m_pluginList.InsertItem(index, _T(""));
		m_pluginList.SetCheck(index, i.m_enable ? TRUE : FALSE);
		m_pluginList.SetItemText(index, 1, i.m_sectionName.c_str());
	}*/
}

// 新建
void CCDConfigDlg::OnBnClickedButton6()
{

}

// 删除
void CCDConfigDlg::OnBnClickedButton5()
{
	int index = m_pluginList.GetSelectionMark();
	if (index == LB_ERR)
		return;

	m_pluginList.DeleteItem(index);
	index = index < m_pluginList.GetItemCount() ? index : index - 1;

	m_pluginList.SetSelectionMark(index);
	m_pluginList.SetItemState(index, LVNI_FOCUSED | LVNI_SELECTED, LVNI_FOCUSED | LVNI_SELECTED);
}

// 上移
void CCDConfigDlg::OnBnClickedButton4()
{
	int index = m_pluginList.GetSelectionMark();
	if (index == LB_ERR || index <= 0)
		return;

	BOOL checked = m_pluginList.GetCheck(index);
	std::vector<CString> texts(2);
	for (int i = 0; i < (int)texts.size(); ++i)
		texts[i] = m_pluginList.GetItemText(index, i);
	m_pluginList.DeleteItem(index);

	m_pluginList.InsertItem(--index, _T(""));
	m_pluginList.SetCheck(index, checked);
	for (int i = 0; i < (int)texts.size(); ++i)
		m_pluginList.SetItemText(index, i, texts[i]);

	m_pluginList.SetSelectionMark(index);
	m_pluginList.SetItemState(index, LVNI_FOCUSED | LVNI_SELECTED, LVNI_FOCUSED | LVNI_SELECTED);
}

// 下移
void CCDConfigDlg::OnBnClickedButton3()
{
	int index = m_pluginList.GetSelectionMark();
	if (index == LB_ERR || index >= m_pluginList.GetItemCount() - 1)
		return;

	BOOL checked = m_pluginList.GetCheck(index);
	std::vector<CString> texts(2);
	for (int i = 0; i < (int)texts.size(); ++i)
		texts[i] = m_pluginList.GetItemText(index, i);
	m_pluginList.DeleteItem(index);

	m_pluginList.InsertItem(++index, _T(""));
	m_pluginList.SetCheck(index, checked);
	for (int i = 0; i < (int)texts.size(); ++i)
		m_pluginList.SetItemText(index, i, texts[i]);

	m_pluginList.SetSelectionMark(index);
	m_pluginList.SetItemState(index, LVNI_FOCUSED | LVNI_SELECTED, LVNI_FOCUSED | LVNI_SELECTED);
}

// 重新加载
void CCDConfigDlg::OnBnClickedButton1()
{
	Save();
	/*ShellExecute(NULL, _T("open"), _T("Inject.exe"), NULL, NULL, SW_SHOWNORMAL);
	ShellExecute(NULL, _T("open"), _T("Inject.exe"), NULL, NULL, SW_SHOWNORMAL);*/
}

void CCDConfigDlg::OnOK()
{
	Save();

	CDialog::OnOK();
}

void CCDConfigDlg::Save()
{
	/*std::wstring path = PluginManager::GetPluginListPath();
	DeleteFileW(path.c_str());

	Plugin plugin;
	for (int i = 0; i < m_pluginList.GetItemCount(); ++i)
	{
		plugin.m_sectionName = m_pluginList.GetItemText(i, 1);
		plugin.m_enable = m_pluginList.GetCheck(i) != FALSE;
		plugin.Save(path);
	}*/
}

