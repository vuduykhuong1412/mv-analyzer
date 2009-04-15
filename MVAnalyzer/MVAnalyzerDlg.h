// MVAnalyzerDlg.h : header file
//

#if !defined(AFX_MVANALYZERDLG_H__5CF3D687_DE3A_40E2_B4BB_92356F153B0D__INCLUDED_)
#define AFX_MVANALYZERDLG_H__5CF3D687_DE3A_40E2_B4BB_92356F153B0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CDXCSIZINGDIALOG.H"
#include "MVplayback.h"
#include "MVreference.h"
#include "FocusArea.h"
#include "FocusRef.h"
#include "Convert.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CMVAnalyzerDlg dialog

// YUV -- Colorful
// YY  -- Gray
// LG  -- LightGray
// ND  -- Not Display
enum { YUV, YY, LG, ND };

class CMVAnalyzerDlg : public cdxCSizingDialog
{
// Construction
public:
	CMVAnalyzerDlg(CWnd* pParent = NULL);	// standard constructor
	void OpenFileA();

// Dialog Data
	//{{AFX_DATA(CMVAnalyzerDlg)
	enum { IDD = IDD_MVANALYZER_DIALOG };
	CButton	m_fullscreen;
	CButton	m_MVscale;
	CStatic	m_static_SADdiff;
	CStatic	m_static_currBLK;
	CStatic	m_static_currSAD;
	CStatic	m_static_minSAD;
	CStatic	m_SADdiff;
	CStatic	m_optSAD;
	CStatic	m_bx;
	CStatic	m_by;
	CStatic	m_currSAD;
	CButton	m_sign;
	CButton	m_file_open;
	CButton	m_prev;
	CButton	m_next;
	CButton	m_playbackBBRight;
	CButton	m_playbackBBLeft;
	CButton	m_playbackBBDown;
	CButton	m_playbackBBUp;
	CButton	m_grid;
	CButton	m_YUV;
	CFocusArea	m_FocusArea;
	CFocusRef	m_FocusRef;
	CButton	m_bMV;
	CMVReference	m_Reference;
	CMVPlayback	m_playback;
	CButton	m_setting;
	CButton	m_play_pause;
	CStatic	m_frame_num;
	CSliderCtrl	m_progress;
	//}}AFX_DATA

	int iWidth,iHeight;	// YUV file width & height setting by m_setting
	int fps;			// frame per second
	UINT playback_timer;// timer for fps
	UINT playback_blink_timer;	// current block blink timer;
	UINT focusarea_blink_timer;	// current MV blink timer;
	BOOL bPlay;			// play or pause;
	int  bYUV;		// display Colorful YUV or Y only or U only or V only
	double MVScaleFactor;	// MV scale factor, x1, x2, x0.5
	int iCurrFrameNumber;	// current frame number, start from 0, 1, 2, ......
	char sPathName[1024];
//	char sFileNameA[128];	// 
//	char sFileNameB[128];	// 
	int iTotalFrameNumber;	//

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMVAnalyzerDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMVAnalyzerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnPlay();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnReleasedcaptureProgress(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetting();
	afx_msg void OnMV();
	afx_msg void OnYUV();
	afx_msg void OnGrid();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNext();
	afx_msg void OnPrev();
	afx_msg void OnPlaybackBbUp();
	afx_msg void OnPlaybackBbDown();
	afx_msg void OnPlaybackBbLeft();
	afx_msg void OnPlaybackBbRight();
	afx_msg void OnSign();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnFileOpen();
	afx_msg void OnMVScale();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void SetTitle(void);
	BOOL IsDraging;
	CPoint DragPntStart;
	void ReStart(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MVAnalyzerDlg_H__5CF3D687_DE3A_40E2_B4BB_92356F153B0D__INCLUDED_)
