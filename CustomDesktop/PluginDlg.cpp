// PluginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PluginDlg.h"


// CPluginDlg 对话框

IMPLEMENT_DYNAMIC(CPluginDlg, CDialog)

CPluginDlg::CPluginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPluginDlg::IDD, pParent)
{

}

CPluginDlg::~CPluginDlg()
{
}

void CPluginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPluginDlg, CDialog)
END_MESSAGE_MAP()


// CPluginDlg 消息处理程序
