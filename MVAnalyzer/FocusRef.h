#if !defined(AFX_FOCUSREF_H__AA1DDFFA_B444_4BE5_936B_DEED9C73883B__INCLUDED_)
#define AFX_FOCUSREF_H__AA1DDFFA_B444_4BE5_936B_DEED9C73883B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FocusRef.h : header file
//
#include "defines.h"
#include "MBData.h"

class CMVAnalyzerDlg;

/////////////////////////////////////////////////////////////////////////////
// CFocusRef window
class CFocusRef : public CStatic
{
// Construction
public:
	CFocusRef();

// Attributes
public:

// Operations
public:
	CMVAnalyzerDlg* pDlg;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFocusRef)
	//}}AFX_VIRTUAL

// Implementation
public:
	void ModifyMV(void);
	void NextCandidate(void);
	void GetCandidates(void);
	void MoveUp(void);
	void MoveDown(void);
	void MoveLeft(void);
	void MoveRight(void);
	void SetSearchArea(CMVReference* p, int bx, int by, MVData* mvd, MVSubMB* vsb);
	virtual ~CFocusRef();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFocusRef)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	double optSAD;
	int CurrCandidate;
	MVData candidates[10];
	int rw, rh;
	int SMBX, SMBY;
	MVSubMB	CurrVSB;
	MVData	CurrMV;
	BOOL	bHave;
	CMVReference *Pic;
	int FocusBlockY;
	int FocusBlockX;
	unsigned char Y[FOCUS_PIX_SIZE][FOCUS_PIX_SIZE];
	unsigned char tempY[FOCUS_PIX_SIZE][FOCUS_PIX_SIZE];
	unsigned char * RGBbuf;
	CMBData	mb;	// block structure
	BITMAPINFO * BmpInfo;

	double CalcCurrSAD(void);
	void GetClientRectSize(void);
	void DrawBackground(CDC *pDC);
	void SetRefArea(void);
	int toWindowX(int x);
	int toWindowY(int y);
	int toPictureX(int x);
	int toPictureY(int y);
	void ShowFocusArea(CDC *pDC);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOCUSREF_H__AA1DDFFA_B444_4BE5_936B_DEED9C73883B__INCLUDED_)
