// SettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MVAnalyzer.h"
#include "SettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettingDlg dialog


CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingDlg)
	m_height = 0;
	m_width = 0;
	//}}AFX_DATA_INIT
}


void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingDlg)
	DDX_Control(pDX, IDC_FPS_LIST, m_fps_list);
	DDX_Control(pDX, IDC_SIZE_LIST, m_size_list);
	DDX_Text(pDX, IDC_SIZE_HEIGHT, m_height);
	DDV_MinMaxUInt(pDX, m_height, 0, 9999);
	DDX_Text(pDX, IDC_SIZE_WIDTH, m_width);
	DDV_MinMaxUInt(pDX, m_width, 0, 9999);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CSettingDlg)
	ON_CBN_SELCHANGE(IDC_SIZE_LIST, OnSelchangeSizeList)
	ON_EN_UPDATE(IDC_SIZE_WIDTH, OnUpdateSizeWidth)
	ON_EN_UPDATE(IDC_SIZE_HEIGHT, OnUpdateSizeHeight)
	ON_CBN_SELCHANGE(IDC_FPS_LIST, OnSelchangeFpsList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingDlg message handlers

BOOL CSettingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_size_list.SetCurSel(3);
	m_fps_list.SetCurSel(6);
	m_width = 352;
	m_height = 288;
	m_fps = 30;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingDlg::OnSelchangeSizeList() 
{
	// TODO: Add your control notification handler code here
	int sel = m_size_list.GetCurSel();
	switch (sel)
	{
	case 0 :
		m_width = 80;
		m_height = 64;
		break;
	case 1 :
		m_width = 88;
		m_height = 72;
		break;
	case 2 :
		m_width = 176;
		m_height = 144;
		break;
	case 3:
		m_width = 352;
		m_height = 288;
		break;
	}
	UpdateData(FALSE);
}

void CSettingDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	
	CDialog::OnOK();
}

void CSettingDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CSettingDlg::OnUpdateSizeWidth() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CSettingDlg::OnUpdateSizeHeight() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CSettingDlg::OnSelchangeFpsList() 
{
	// TODO: Add your control notification handler code here
	int fps = m_fps_list.GetCurSel();
	switch (fps)
	{
	case 0 :
		m_fps = 1;
		break;
	case 1 :
		m_fps = 2;
		break;
	case 2 :
		m_fps = 5;
		break;
	case 3 :
		m_fps = 10;
		break;
	case 4 :
		m_fps = 15;
		break;
	case 5 :
		m_fps = 25;
		break;
	case 6 :
		m_fps = 30;
		break;
	}
}
