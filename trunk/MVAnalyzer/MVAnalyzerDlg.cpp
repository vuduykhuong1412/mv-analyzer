// MVAnalyzerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MVAnalyzer.h"
#include "MVAnalyzerDlg.h"
#include "SettingDlg.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()











/////////////////////////////////////////////////////////////////////////////
// CMVAnalyzerDlg dialog

CMVAnalyzerDlg::CMVAnalyzerDlg(CWnd* pParent /*=NULL*/)
	: cdxCSizingDialog(CMVAnalyzerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMVAnalyzerDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	bPlay = FALSE;
	bYUV = YUV;

	m_playback.SetColorful(bYUV);
	m_Reference.SetColorful(bYUV);
	iWidth = 352; iHeight = 288;
	iCurrFrameNumber = 0;
	iTotalFrameNumber = 0;
	fps = 30;

	IsDraging = FALSE;

	m_playback.pDlg = this;
	m_FocusArea.pDlg = this;
	m_FocusRef.pDlg = this;

	return;
}

void CMVAnalyzerDlg::DoDataExchange(CDataExchange* pDX)
{
	cdxCSizingDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMVAnalyzerDlg)
	DDX_Control(pDX, IDC_STATIC_SADDIFF, m_static_SADdiff);
	DDX_Control(pDX, IDC_STATIC_CURRBLK, m_static_currBLK);
	DDX_Control(pDX, IDC_STATIC_CURRSAD, m_static_currSAD);
	DDX_Control(pDX, IDC_STATIC_MINSAD, m_static_minSAD);
	DDX_Control(pDX, IDC_SAD_DIFF, m_SADdiff);
	DDX_Control(pDX, IDC_OPT_SAD, m_optSAD);
	DDX_Control(pDX, IDC_CURR_BLK_X, m_bx);
	DDX_Control(pDX, IDC_CURR_BLK_Y, m_by);
	DDX_Control(pDX, IDC_Curr_SAD, m_currSAD);
	DDX_Control(pDX, IDC_SIGN, m_sign);
	DDX_Control(pDX, IDC_FILE_OPEN, m_file_open);
	DDX_Control(pDX, IDC_PREV, m_prev);
	DDX_Control(pDX, IDC_NEXT, m_next);
	DDX_Control(pDX, IDC_PLAYBACK_BB_RIGHT, m_playbackBBRight);
	DDX_Control(pDX, IDC_PLAYBACK_BB_LEFT, m_playbackBBLeft);
	DDX_Control(pDX, IDC_PLAYBACK_BB_DOWN, m_playbackBBDown);
	DDX_Control(pDX, IDC_PLAYBACK_BB_UP, m_playbackBBUp);
	DDX_Control(pDX, IDC_GRID, m_grid);
	DDX_Control(pDX, IDC_YUV, m_YUV);
	DDX_Control(pDX, IDC_PLAYBACK_AA, m_FocusArea);
	DDX_Control(pDX, IDC_PLAYBACK_BB, m_FocusRef);
	DDX_Control(pDX, IDC_MV, m_bMV);
	DDX_Control(pDX, IDC_PLAYBACK_B, m_Reference);
	DDX_Control(pDX, IDC_PLAYBACK_A, m_playback);
	DDX_Control(pDX, IDC_SETTING, m_setting);
	DDX_Control(pDX, IDC_PLAY, m_play_pause);
	DDX_Control(pDX, IDC_FRAME_NUM, m_frame_num);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMVAnalyzerDlg, cdxCSizingDialog)
	//{{AFX_MSG_MAP(CMVAnalyzerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_WM_TIMER()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_PROGRESS, OnReleasedcaptureProgress)
	ON_BN_CLICKED(IDC_SETTING, OnSetting)
	ON_BN_CLICKED(IDC_MV, OnMV)
	ON_BN_CLICKED(IDC_YUV, OnYUV)
	ON_BN_CLICKED(IDC_GRID, OnGrid)
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_PREV, OnPrev)
	ON_BN_CLICKED(IDC_PLAYBACK_BB_UP, OnPlaybackBbUp)
	ON_BN_CLICKED(IDC_PLAYBACK_BB_DOWN, OnPlaybackBbDown)
	ON_BN_CLICKED(IDC_PLAYBACK_BB_LEFT, OnPlaybackBbLeft)
	ON_BN_CLICKED(IDC_PLAYBACK_BB_RIGHT, OnPlaybackBbRight)
	ON_BN_CLICKED(IDC_SIGN, OnSign)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMVAnalyzerDlg message handlers

BOOL CMVAnalyzerDlg::OnInitDialog()
{
	cdxCSizingDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	AddSzControlEx(m_playback, 50, 100, 50, 100); m_playback.ShowWindow(SW_SHOWNORMAL);
	AddSzControlEx(m_Reference, 50, 100, 0, 50); m_Reference.ShowWindow(SW_SHOWNORMAL);
	AddSzControlEx(m_FocusArea, 0, 50, 50, 100); m_FocusArea.ShowWindow(SW_SHOWNORMAL);
	AddSzControlEx(m_FocusRef, 0, 50, 0, 50); m_FocusRef.ShowWindow(SW_SHOWNORMAL);
	AddSzControlEx(m_playbackBBUp, 0, 50, 0, 0);
	AddSzControlEx(m_playbackBBDown, 0, 50, 50, 50);
	AddSzControlEx(m_playbackBBLeft, 0, 0, 0, 50);
	AddSzControlEx(m_playbackBBRight, 50, 50, 0, 50);
	AddSzControl(m_file_open, mdRepos, mdRepos);
	AddSzControl(m_play_pause, mdRepos, mdRepos);	m_play_pause.SetWindowText("GO");
	AddSzControl(m_prev, mdRepos, mdRepos);
	AddSzControl(m_next, mdRepos, mdRepos);
	AddSzControl(m_YUV, mdRepos, mdRepos);	m_YUV.SetWindowText("YUV");
	AddSzControl(m_grid, mdRepos, mdRepos);	m_grid.SetCheck(BST_UNCHECKED);
	AddSzControl(m_bMV, mdRepos, mdRepos);
	AddSzControl(m_sign, mdRepos, mdRepos);
	AddSzControl(m_setting, mdRepos, mdRepos);
	AddSzControlEx(m_progress, 0, 50, 100, 100);	m_progress.SetRange(0, iTotalFrameNumber-1);
	AddSzControlEx(m_frame_num, 50, 50, 100, 100);

	AddSzControl(m_optSAD, mdRelative, mdNone);
	AddSzControl(m_static_minSAD, mdRelative, mdNone);
	AddSzControl(m_currSAD, mdRelative, mdNone);
	AddSzControl(m_static_currSAD, mdRelative, mdNone);
	AddSzControl(m_SADdiff, mdRelative, mdNone);
	AddSzControl(m_static_SADdiff, mdRelative, mdNone);
	AddSzControl(m_bx, mdRelative, mdNone);
	AddSzControl(m_by, mdRelative, mdNone);
	AddSzControl(m_static_currBLK, mdRelative, mdNone);

	RestoreWindowPosition(_T("Main\\Window"));
	MoveWindow(0, 0, 800, 600);
	CenterWindow();

	playback_blink_timer = SetTimer(1112, 1000/4, NULL);
	focusarea_blink_timer = SetTimer(1113, 1000/4, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CMVAnalyzerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		cdxCSizingDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMVAnalyzerDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		cdxCSizingDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMVAnalyzerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMVAnalyzerDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	StoreWindowPosition(CString(_T("Main\\Window")));

	cdxCSizingDialog::OnClose();
}

void CMVAnalyzerDlg::OpenFileA() 
{
	// TODO: Add your control notification handler code here

	// get pathname & filename
	m_playback.SetPathName(sFileNameA);
	////AfxMessageBox(sFileNameA, MB_OK);

	CFile *pFile = new CFile();
	if(!pFile->Open(sFileNameA, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone )) 
	{
		//AfxMessageBox("Can't open input file");
		delete pFile;
		return;
	}
	iTotalFrameNumber = pFile->GetLength() / (iWidth*iHeight*3/2);
	iCurrFrameNumber = 0;
	pFile->Close();
	delete pFile;

	ReStart();
}

void CMVAnalyzerDlg::OnNext() 
{
	// TODO: Add your control notification handler code here
	iCurrFrameNumber++;
	if (iCurrFrameNumber>=iTotalFrameNumber) {
		iCurrFrameNumber = iTotalFrameNumber-1;
		return;
	}

	char sFrameNumber[16];
	sprintf( sFrameNumber, "%d/%d", iCurrFrameNumber+1, iTotalFrameNumber );
	m_frame_num.SetWindowText(sFrameNumber);
	m_progress.SetPos(iCurrFrameNumber);

	if (iCurrFrameNumber<iTotalFrameNumber) {
		m_playback.GoToFrame(iCurrFrameNumber);
		m_Reference.GoToFrame(iCurrFrameNumber-1);
	}

	Invalidate(true);

	SetTitle();
}

void CMVAnalyzerDlg::OnPrev() 
{
	// TODO: Add your control notification handler code here
	iCurrFrameNumber--;
	if (iCurrFrameNumber<0) {
		iCurrFrameNumber = 0;
		return;
	}

	char sFrameNumber[16];
	sprintf( sFrameNumber, "%d/%d", iCurrFrameNumber+1, iTotalFrameNumber );
	m_frame_num.SetWindowText(sFrameNumber);
	m_progress.SetPos(iCurrFrameNumber);

	if (iCurrFrameNumber<=iTotalFrameNumber-1) {
		m_playback.GoToFrame(iCurrFrameNumber);
		m_Reference.GoToFrame(iCurrFrameNumber-1);
	}

	SetTitle();
}

void CMVAnalyzerDlg::OnReleasedcaptureProgress(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	iCurrFrameNumber = m_progress.GetPos();

	char sFrameNumber[16];
	sprintf( sFrameNumber, "%d/%d", iCurrFrameNumber+1, iTotalFrameNumber );
	m_frame_num.SetWindowText(sFrameNumber);

	m_playback.GoToFrame(iCurrFrameNumber);
	m_Reference.GoToFrame(iCurrFrameNumber-1);

	Invalidate(TRUE);

	*pResult = 0;
}

void CMVAnalyzerDlg::OnPlay() 
{
	// TODO: Add your control notification handler code here
	if (bPlay == FALSE) {
		if (playback_timer == 1111)
			KillTimer(playback_timer);

		playback_timer = SetTimer(1111, 1000/fps, NULL);

		bPlay = TRUE;
		m_play_pause.SetWindowText("II");
	}
	else // bPlay == TRUE
	{
		if (playback_timer == 1111)
			KillTimer(playback_timer);

		bPlay = FALSE;
		m_play_pause.SetWindowText("GO");
	}
}

BOOL CMVAnalyzerDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam) {
		case 38: // up
			m_FocusRef.MoveUp();
			break;
		case 40: // down
			m_FocusRef.MoveDown();
			break;
		case 37: // left
			m_FocusRef.MoveLeft();
			break;
		case 39: // right
			m_FocusRef.MoveRight();
			break;
		case 33: // page up
			m_playback.PrevQMB();
			break;
		case 34: // page down
			m_playback.NextQMB();
			break;
		case 81: // 'q'
			m_FocusArea.NextMV();
			break;
		case 65: // 'a'
			m_FocusRef.NextCandidate();
			break;
		case 90: // 'z'
			m_FocusRef.ModifyMV();
			break;
		case 27: // ESC
			break;
		default:
			break;
		}
		SetTitle();
		return TRUE;
	}
	if (pMsg->message == WM_DROPFILES)
	{
//		if (pMsg->hwnd == m_playback) {
			char sPathName[1024];
			DragQueryFile(HDROP(pMsg->wParam), 0, sPathName, 128);
			m_playback.SetPathName(sPathName);
			m_Reference.SetPathName(sPathName);
			CFile *pFile = new CFile();
			if(!pFile->Open(sPathName, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone )) 
			{
				//AfxMessageBox("Can't open input file");
				delete pFile;
				return 0;
			}
			iTotalFrameNumber = pFile->GetLength() / (iWidth*iHeight*3/2);
			iCurrFrameNumber = 0;
			pFile->Close();
			delete pFile;

			ReStart();
			SetTitle();
//		}
	}

	return cdxCSizingDialog::PreTranslateMessage(pMsg);
}

void CMVAnalyzerDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == playback_timer) {

		char sFrameNumber[16];
		sprintf( sFrameNumber, "%d/%d", iCurrFrameNumber+1, iTotalFrameNumber );
		m_frame_num.SetWindowText(sFrameNumber);
		m_progress.SetPos(iCurrFrameNumber);

		if (iCurrFrameNumber>=0 && iCurrFrameNumber<iTotalFrameNumber) {
			m_playback.GoToFrame(iCurrFrameNumber);
			m_Reference.GoToFrame(iCurrFrameNumber-1);
		}

		iCurrFrameNumber++;
		if (iCurrFrameNumber<0 || iCurrFrameNumber>=iTotalFrameNumber) {
			OnPlay();
			iCurrFrameNumber = 0;
			return;
		}
	} else if (nIDEvent == playback_blink_timer) {
		m_playback.bBlink = (m_playback.bBlink+1)%2;
		m_playback.Invalidate(FALSE);
	} else if (nIDEvent == focusarea_blink_timer) {
		m_FocusArea.bBlink = (m_FocusArea.bBlink+1)%2;
		m_FocusArea.Invalidate(FALSE);
	}

	cdxCSizingDialog::OnTimer(nIDEvent);
}

void CMVAnalyzerDlg::OnSetting() 
{
	// TODO: Add your control notification handler code here
	CSettingDlg set_dlg;

	int nResponse = set_dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
		fps = set_dlg.m_fps;
		iWidth = set_dlg.m_width;
		iHeight = set_dlg.m_height;

		ReStart();
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
}

void CMVAnalyzerDlg::ReStart()
{
	if (playback_timer == 1111)
		KillTimer(playback_timer);
	bPlay = FALSE;
	m_play_pause.SetWindowText("GO");

	m_playback.SetYUVSize(iWidth, iHeight);
	m_Reference.SetYUVSize(iWidth, iHeight);
	iTotalFrameNumber = m_playback.ReStart();
	iTotalFrameNumber = m_Reference.ReStart();

	char sFrameNumber[16];
	sprintf( sFrameNumber, "%d/%d", iCurrFrameNumber+1, iTotalFrameNumber );
	m_frame_num.SetWindowText(sFrameNumber);
	m_progress.SetRange(0, iTotalFrameNumber-1);
	iCurrFrameNumber = 0;
	m_progress.SetPos(iCurrFrameNumber);

	Invalidate(TRUE);
}

void CMVAnalyzerDlg::OnMV() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_playback.SetShowMV( m_bMV.GetState()&0x0003 );
	Invalidate(TRUE);
}

void CMVAnalyzerDlg::OnYUV() 
{
	// TODO: Add your control notification handler code here
	if ( bYUV == YUV ) {
		bYUV = YY;
		m_YUV.SetWindowText("Y");
		m_playback.SetColorful( bYUV );
		m_Reference.SetColorful( bYUV );
	} else if ( bYUV == YY ) {
		bYUV = LG;
		m_YUV.SetWindowText("LG");
		m_playback.SetColorful( bYUV );
		m_Reference.SetColorful( YY );
	} else if ( bYUV == LG ) {
		bYUV = ND;
		m_YUV.SetWindowText("ND");
		m_playback.SetColorful( bYUV );
		m_Reference.SetColorful( YUV );
	} else if ( bYUV == ND ) {
		bYUV = YUV;
		m_YUV.SetWindowText("YUV");
		m_playback.SetColorful( bYUV );
		m_Reference.SetColorful( bYUV );
	}

	Invalidate(TRUE);
}

void CMVAnalyzerDlg::OnGrid() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_playback.SetShowGrid( m_grid.GetState()&0x0003 );
	Invalidate(TRUE);
}

void CMVAnalyzerDlg::OnSign() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_playback.SetShowSign( m_sign.GetState()&0x0003 );
	Invalidate(TRUE);
}

BOOL CMVAnalyzerDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	if (zDelta>0) {
		m_playback.ZoomIn();
		m_Reference.ZoomIn();
	} else {
		m_playback.ZoomOut();
		m_Reference.ZoomOut();
	}

	return cdxCSizingDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CMVAnalyzerDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	IsDraging = TRUE;
	DragPntStart = point;
	m_playback.DragStart();
	m_Reference.DragStart();

	cdxCSizingDialog::OnLButtonDown(nFlags, point);
}

void CMVAnalyzerDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	IsDraging = FALSE;
	
	cdxCSizingDialog::OnLButtonUp(nFlags, point);
}

void CMVAnalyzerDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	CSize move;
	// TODO: Add your message handler code here and/or call default
	if (IsDraging) {
		move = point - DragPntStart;
		m_playback.Drag(move.cx, move.cy);
		m_Reference.Drag(move.cx, move.cy);
	}
	
	cdxCSizingDialog::OnMouseMove(nFlags, point);
}

void CMVAnalyzerDlg::OnPlaybackBbUp() 
{
	// TODO: Add your control notification handler code here
}

void CMVAnalyzerDlg::OnPlaybackBbDown() 
{
	// TODO: Add your control notification handler code here
}

void CMVAnalyzerDlg::OnPlaybackBbLeft() 
{
	// TODO: Add your control notification handler code here
}

void CMVAnalyzerDlg::OnPlaybackBbRight() 
{
	// TODO: Add your control notification handler code here
}

void CMVAnalyzerDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CRect   rect;
	m_playback.GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_playback.SelectQMB(point.x-rect.left, point.y-rect.top);
	
	cdxCSizingDialog::OnLButtonDblClk(nFlags, point);
}

void CMVAnalyzerDlg::SetTitle()
{
	char sTitle[256];
	sprintf(sTitle, "MV-Analyzer::%s--[%d-%d]", m_playback.GetFileName(), iTotalFrameNumber, iCurrFrameNumber);
	SetWindowText(sTitle);

	sprintf(sTitle, "BX=%d", m_playback.QMB_bx);
	m_bx.SetWindowText(sTitle);
	sprintf(sTitle, "BY=%d", m_playback.QMB_by);
	m_by.SetWindowText(sTitle);
}
