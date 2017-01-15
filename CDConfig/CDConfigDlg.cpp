// CDConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CDConfigDlg.h"
#include "ComboDlg.h"
#include <memory>
#include <vector>


// CCDConfigDlg 对话框

CCDConfigDlg::CCDConfigDlg(CWnd* pParent /*=NULL*/) : 
	CDialog(CCDConfigDlg::IDD, pParent)
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

// 新建
void CCDConfigDlg::OnBnClickedButton6()
{
	std::vector<CString> list;
	CFileFind find;
	BOOL found = find.FindFile(_T("Plugin\\*.dll"));
	while (found)
	{
		found = find.FindNextFile();
		CString name = find.GetFileTitle();
		for (int i = 0; i < m_pluginList.GetItemCount(); ++i)
		{
			if (m_pluginList.GetItemText(i, 1).CompareNoCase(name) == 0)
				goto NextFile;
		}
		list.push_back(name);
	NextFile:
		;
	}

	if (list.empty())
	{
		AfxMessageBox(_T("在Plugin目录下没有找到新插件"), MB_ICONINFORMATION);
		return;
	}

	CComboDlg dlg(list, this);
	if (dlg.DoModal() == IDOK && dlg.m_sel != -1)
	{
		int index = m_pluginList.GetSelectionMark();
		if (index == LB_ERR)
			index = 0;

		index = m_pluginList.InsertItem(index, _T(""));
		m_pluginList.SetCheck(index, TRUE);
		m_pluginList.SetItemText(index, 1, list[dlg.m_sel]);

		m_pluginList.SetSelectionMark(index);
		m_pluginList.SetItemState(index, LVNI_FOCUSED | LVNI_SELECTED, LVNI_FOCUSED | LVNI_SELECTED);
	}
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

	SHELLEXECUTEINFO info = {};
	info.cbSize = sizeof(info);
	info.fMask = SEE_MASK_NOCLOSEPROCESS;
	info.lpVerb = _T("open");
	info.lpFile = _T("Inject.exe");
	info.nShow = SW_SHOWNORMAL;

	ShellExecuteEx(&info);
	WaitForSingleObject(info.hProcess, INFINITE);
	CloseHandle(info.hProcess);
	ShellExecuteEx(&info);
	WaitForSingleObject(info.hProcess, INFINITE);
	CloseHandle(info.hProcess);
}

void CCDConfigDlg::OnOK()
{
	Save();

	CDialog::OnOK();
}


CString CCDConfigDlg::GetPluginListPath()
{
	CString path;
	GetCurrentDirectory(MAX_PATH, path.GetBuffer(MAX_PATH));
	path.ReleaseBuffer();
	return path + _T("\\Plugins.ini");
}

void CCDConfigDlg::ShowPlugins()
{
	m_pluginList.DeleteAllItems();

	// 见CustomDesktop\PluginManager.cpp PluginManager::LoadPluginList

	CString path = GetPluginListPath();
	auto buffer = std::make_unique<TCHAR[]>(10240);
	DWORD size = GetPrivateProfileSectionNames(buffer.get(), 10240, path);
	CString strName;
	for (LPCTSTR name = buffer.get(); name < buffer.get() + size; name += strName.GetLength() + 1)
	{
		strName = name;

		int index = m_pluginList.InsertItem(m_pluginList.GetItemCount(), _T(""));
		m_pluginList.SetCheck(index, GetPrivateProfileInt(strName, _T("Enable"), 1, path) != 0);
		m_pluginList.SetItemText(index, 1, strName);
	}
}

void CCDConfigDlg::Save()
{
	CString path = GetPluginListPath();
	DeleteFileW(path);

	for (int i = 0; i < m_pluginList.GetItemCount(); ++i)
	{
		WritePrivateProfileString(m_pluginList.GetItemText(i, 1), _T("Enable"), m_pluginList.GetCheck(i) ? _T("1") : _T("0"), path);
	}
}
