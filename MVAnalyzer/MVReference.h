#if !defined(AFX_MVREFERENCE_H__DD73A99A_58DA_4FD0_817A_C96C17A630B9__INCLUDED_)
#define AFX_MVREFERENCE_H__DD73A99A_58DA_4FD0_817A_C96C17A630B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MVReference.h : header file
//

#include "defines.h"

/////////////////////////////////////////////////////////////////////////////
// CMVReference window
class CMVReference : public CStatic
{
// Construction
public:
	CMVReference();

// Attributes
public:

// Operations
public:
	int		iWidth;
	int		iHeight;
	unsigned char * Y, * Cb, * Cr;

private:
	BOOL	bHaveFile;
	char	sPathName[1024];
	CFile *	m_pFile;
	int		iTotalFrameNumber;
	int		iCurrFrameNumber;
	int		bColorful;
	unsigned char * RGBbuf;
	BITMAPINFO * BmpInfo;
	// record the drag start positions
	int DragStart_CenterX, DragStart_CenterY, DragStart_x_left, DragStart_x_right, DragStart_y_top, DragStart_y_bottom;
	// current 4 corner & center positions in YUV picture
	int CenterX, CenterY, x_left, x_right, y_top, y_bottom;
	// the window width & height, and x,y white edges of this reference window
	int rw, rh, edge_x, edge_y; 
	double ZoomFactor;
	int RefXleft, RefXright, RefYtop, RefYbottom;

	void DrawRefArea(CDC *pDC);
	void DrawBackground(CDC *pDC);
	void ReCalulateShowParam(void);
	void GetClientRectSize(void);
	int toPictureY(int y);
	int toPictureX(int x);
	int toWindowY(int y);
	int toWindowX(int x);
	void ShowColorImage(CDC *pDC);
	void ShowYImage(CDC *pDC);
	void GetYUVData(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMVReference)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetRefArea(int x1, int y1, int x2, int y2);
	void SetColorful(int colorful);
	void ZoomIn(void);
	void ZoomOut(void);
	void DragStart(void);
	void Drag(int x, int y);
	void SetYUVSize(int width, int height);
	void SetPathName(char * pathname);
	int ReStart(void);
	void GoToFrame(int number);

	virtual ~CMVReference();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMVReference)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MVREFERENCE_H__DD73A99A_58DA_4FD0_817A_C96C17A630B9__INCLUDED_)
