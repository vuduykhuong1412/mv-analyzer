#if !defined(AFX_SETTINGDLG_H__4E786CAA_0212_4332_A6FE_3AD6E9D036F1__INCLUDED_)
#define AFX_SETTINGDLG_H__4E786CAA_0212_4332_A6FE_3AD6E9D036F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSettingDlg dialog

class CSettingDlg : public CDialog
{
// Construction
public:
	CSettingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSettingDlg)
	enum { IDD = IDD_SETTING };
	CComboBox	m_fps_list;
	CComboBox	m_size_list;
	UINT	m_fps;
	UINT	m_height;
	UINT	m_width;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSettingDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeSizeList();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnUpdateSizeWidth();
	afx_msg void OnUpdateSizeHeight();
	afx_msg void OnSelchangeFpsList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGDLG_H__4E786CAA_0212_4332_A6FE_3AD6E9D036F1__INCLUDED_)
