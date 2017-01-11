#pragma once
#include "resource.h"
#include "ModelessDlg.h"


// CPluginDlg 对话框

class CPluginDlg final : public CModelessDlg<CPluginDlg>
{
	DECLARE_DYNAMIC(CPluginDlg)

public:
	CPluginDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPluginDlg();

	// 对话框数据
	enum { IDD = IDD_PLUGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();

	void ShowPlugins();


public:
	CListCtrl m_pluginList;
};
