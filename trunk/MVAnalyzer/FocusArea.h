#if !defined(AFX_FOCUSAREA_H__C5815FC7_9857_444A_8123_AA176B78EA85__INCLUDED_)
#define AFX_FOCUSAREA_H__C5815FC7_9857_444A_8123_AA176B78EA85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FocusArea.h : header file
//
#include "defines.h"
#include "MBData.h"

class CMVAnalyzerDlg;

/////////////////////////////////////////////////////////////////////////////
// CFocusArea window
class CFocusArea : public CStatic
{
// Construction
public:
	CFocusArea();

// Attributes
public:

// Operations
public:
	int	bBlink;
	CMVAnalyzerDlg* pDlg;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFocusArea)
	//}}AFX_VIRTUAL

// Implementation
public:
	void ModifyMV(MVData* new_mv);
	unsigned char Y[FOCUS_PIX_SIZE][FOCUS_PIX_SIZE];
	MVData * GetCurrMV(void);
	void NextMV(void);
	void SetFocusArea(CMBData *mbd, int bx, int by);
	virtual ~CFocusArea();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFocusArea)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CMVPlayback *Pic;
	int FocusBlockY;
	int FocusBlockX;
	unsigned char * RGBbuf;
	CMBData	*qmb;	// block structure
	BITMAPINFO * BmpInfo;	void GetClientRectSize(void);
	int rw, rh;
	int CurrMV_No;
	MVData CurrMV;

	void DrawBackground(CDC *pDC);
	void DrawBlock(CDC *pDC, int x1, int y1, int x2, int y2, int Q);
	void DrawArrow(CDC *pDC, int vx, int vy, double cx, double cy, int Q);
	int toWindowX(double x);
	int toWindowY(double y);
	int toPictureX(int x);
	int toPictureY(int y);
	void ShowFocusBlock(CDC *pDC);
	void ShowFocusArea(CDC *pDC);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOCUSAREA_H__C5815FC7_9857_444A_8123_AA176B78EA85__INCLUDED_)
