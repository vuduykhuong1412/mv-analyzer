////////////////////////////////////////////////////////////////////////////
//	Copyright: A. Riazi (3 June 2003)
//
//	Email: a.riazi@misbah3com.com
//
//	This code may be used in compiled form in any way you desire. This
//	file may be redistributed unmodified by any means PROVIDING it is 
//	not sold for profit without the authors written consent, and 
//	providing that this notice and the authors name is included.
//
//	This file is provided 'as is' with no expressed or implied warranty.
//	The author accepts no liability if it causes any damage to your computer.
//
//	Do expect bugs.
//	Please let me know of any bugs/mods/improvements.
//	and I will try to fix/incorporate them into this file.
//	Enjoy!
//
//	Description: CStatic Derived Class for playing Gif Animated files.
//
//  Credit: Most of works done by Juan Soulie <jsoulie@cplusplus.com>
////////////////////////////////////////////////////////////////////////////

#ifndef _MVPLAYBACK_H_
#define _MVPLAYBACK_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MVPlayback.h : header file
//
#include "defines.h"
#include "MBData.h"
#include "tinyxml.h"

class CMVAnalyzerDlg;

/////////////////////////////////////////////////////////////////////////////
// CMVPlayback window

class CMVPlayback : public CStatic
{
// Construction
public:
	CMVPlayback();

	//{{AFX_DATA(CMVAnalyzerDlg)
	enum { IDD = 0 };
	//}}AFX_DATA

// Attributes
public:
	
// Operations
public:
	BOOL	bChanged;
	int		winID;
	int		TotalQMB, QMB_bx, QMB_by;
	int		bBlink;
	int		iWidth;
	int		iHeight;
	unsigned char * Y, * Cb, * Cr;
	CMVAnalyzerDlg* pDlg;

protected:

public:

private:
	BOOL	bHaveFile;
	char	sPathName[1024];
	char	sMVPathName[1024];
	CFile *	m_pFile;
	int		iTotalFrameNumber;
	int		iCurrFrameNumber;
	int		bColorful;
	BOOL	bShowMV;
	BOOL	bShowGrid;
	BOOL	bShowSign;
	unsigned char * RGBbuf;
	BITMAPINFO * BmpInfo;
	// record the drag start positions
	int DragStart_CenterX, DragStart_CenterY, DragStart_x_left, DragStart_x_right, DragStart_y_top, DragStart_y_bottom;
	// current 4 corner & center positions in YUV picture
	int CenterX, CenterY, x_left, x_right, y_top, y_bottom;
	// the window width & height, and x,y white edges of this reference window
	int rw, rh, edge_x, edge_y; 
	double	ZoomFactor;
	CMBData	*QMB;	// MBs with questionable MV of the current picture
	double	MVScaleFactor;

	TiXmlDocument	*pMVFile;
	TiXmlNode	*pXMLFrame;
	void ReadMVfromXML(TiXmlNode* node);
	void GetMVfromXML(MVData* mv, TiXmlNode* mv_node);
	int GetMBtype(const char *s);
	void ModifyCurrMV(CMBData* mbd, int no, int mv_no, TiXmlNode *mv_node);

	void DrawMV(CDC *pDC, int cx, int cy, double vx, double vy, int mode);
//	void MarkIt(MVData *mv); // should be deleted finally.
//	void CreateOneQMB(int bx, int by); // should be deleted finally.
	void FindQuestionableMV(void);
	void DrawBackground(CDC *pDC);
	void ReCalulateShowParam(void);
	void GetClientRectSize(void);
	void DrawRectangle(CDC *pDC, int x1, int x2, int y1, int y2);
	double toPictureY(int y);
	double toPictureX(int x);
	int toWindowY(double y);
	int toWindowX(double x);
	void DrawBlockSign(CDC *pDC, int xl, int yt, int xr, int yb, int Q);
	void ShowMVs(CDC *pDC);
	void ShowGrids(CDC *pDC);
	void ShowSigns(CDC *pDC);
	void ShowFocus(CDC *pDC);
	void ShowColorImage(CDC *pDC);
	void ShowYImage(CDC *pDC);
	void ShowLGImage(CDC *pDC);
	void ShowNDImage(CDC *pDC);
	void GetYUVData(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMVPlayback)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void SaveMVFile(void);
	void MVScale(double f);
	void ModifyMV(int bx, int by, CMBData* mbd, int mv_no);
	void SelectQMB(int x, int y);
	void NextQMB(void);
	void PrevQMB(void);
	void SetColorful(int colorful);
	void SetShowMV(BOOL bMV);
	void SetShowGrid(BOOL grid);
	void SetShowSign(BOOL sign);
	void ZoomIn(void);
	void ZoomOut(void);
	void DragStart(void);
	void Drag(int x, int y);
	void SetYUVSize(int width, int height);
	void SetPathName(char * pathname);
	char* GetFileName(void);
	void GoToFrame(int number);
	int ReStart(void);

	virtual ~CMVPlayback();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMVPlayback)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _GIF_ANIMATION_H_
