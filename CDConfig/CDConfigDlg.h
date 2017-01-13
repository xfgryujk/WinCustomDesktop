#pragma once
#include "resource.h"


// CCDConfigDlg 对话框

class CCDConfigDlg : public CDialog
{
// 构造
public:
	CCDConfigDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CCDConfigDlg() = default;

// 对话框数据
	enum { IDD = IDD_PLUGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton1();
	virtual void OnOK();

	void ShowPlugins();
	void Save();


public:
	CListCtrl m_pluginList;
};
