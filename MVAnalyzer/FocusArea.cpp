// FocusArea.cpp : implementation file
//

#include "stdafx.h"
#include "MVanalyzer.h"
#include "MVPlayback.h"
#include "FocusArea.h"
#include "MVAnalyzerDlg.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFocusArea

CFocusArea::CFocusArea()
{
	FocusBlockX = -1;
	FocusBlockY = -1;
	qmb = NULL;
	CurrMV_No = -1;

	BmpInfo=(BITMAPINFO*)new char [sizeof(BITMAPINFO)+sizeof(RGBQUAD)*256];

	bBlink = 1;
}

CFocusArea::~CFocusArea()
{
	delete BmpInfo;
}


BEGIN_MESSAGE_MAP(CFocusArea, CStatic)
	//{{AFX_MSG_MAP(CFocusArea)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFocusArea message handlers

void CFocusArea::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	ShowFocusArea(&dc);
}

void CFocusArea::ShowFocusArea(CDC *pDC)
{
	GetClientRectSize();
	CDC MemDC; CDC *pMemDC; pMemDC = &MemDC; MemDC.CreateCompatibleDC(pDC);  
	CBitmap bmp; bmp.CreateCompatibleBitmap(pDC,rw,rh); MemDC.SelectObject(&bmp);

	DrawBackground(pMemDC);

	int i;         
	HANDLE hMem;
	BYTE *lpBuf;

	int BPP = 8;

	BmpInfo->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	BmpInfo->bmiHeader.biWidth=FOCUS_PIX_SIZE;
	BmpInfo->bmiHeader.biHeight=FOCUS_PIX_SIZE;			// negative means up-to-bottom 
	BmpInfo->bmiHeader.biPlanes=1;
	BmpInfo->bmiHeader.biBitCount=(BPP<8?8:BPP);	// Our raster is byte-aligned
	BmpInfo->bmiHeader.biCompression=BI_RGB;
	BmpInfo->bmiHeader.biSizeImage=0;
	BmpInfo->bmiHeader.biXPelsPerMeter=11811;
	BmpInfo->bmiHeader.biYPelsPerMeter=11811;
	BmpInfo->bmiHeader.biClrUsed=0;
	BmpInfo->bmiHeader.biClrImportant=0;

	HANDLE hloc1;
	RGBQUAD *argbq;
	hloc1 = LocalAlloc(LMEM_ZEROINIT | LMEM_MOVEABLE,(sizeof(RGBQUAD) * 256));
	argbq = (RGBQUAD *) LocalLock(hloc1);
	for(i=0;i<256;i++) {
		argbq[i].rgbBlue=i;
		argbq[i].rgbGreen=i;
		argbq[i].rgbRed=i;
		argbq[i].rgbReserved=0;
	}
	memcpy(BmpInfo->bmiColors, argbq, sizeof(RGBQUAD) * 256);
	LocalUnlock(hloc1);
	LocalFree(hloc1);

    hMem=GlobalAlloc(GHND, FOCUS_PIX_SIZE*FOCUS_PIX_SIZE);
	lpBuf=(BYTE *)GlobalLock(hMem);
	
    //将倒置的图象放正 
	//Make the inverse image normal
	for(i=0;i<FOCUS_PIX_SIZE;i++) {
		memcpy(lpBuf+FOCUS_PIX_SIZE*(FOCUS_PIX_SIZE-1-i), Y[i], FOCUS_PIX_SIZE);
	}

	pMemDC->SetStretchBltMode(STRETCH_DELETESCANS);
	StretchDIBits(pMemDC->m_hDC,0,0,rw,rh,0,0,FOCUS_PIX_SIZE,FOCUS_PIX_SIZE,
				  lpBuf, BmpInfo, DIB_RGB_COLORS, SRCCOPY);  

	GlobalUnlock(hMem);
	GlobalFree(hMem);

	ShowFocusBlock(pMemDC);

	pDC->BitBlt(0, 0, rw, rh, pMemDC, 0, 0, SRCCOPY);
	MemDC.DeleteDC();  
	// Do not call CWnd::OnPaint() for painting messages  
}

void CFocusArea::SetFocusArea(CMBData *mbd, int bx, int by)
{
	qmb = mbd;
	FocusBlockX = bx;
	FocusBlockY = by;
	CurrMV_No = 0;

	if (qmb == NULL || FocusBlockX < 0 || FocusBlockY < 0)
		return;

	Pic = &(pDlg->m_playback);
	unsigned char val;
	for (int y=by*MB_SIZE-FOCUS_RANGE; y<by*MB_SIZE+MB_SIZE+FOCUS_RANGE; y++) {
		for (int x=bx*MB_SIZE-FOCUS_RANGE; x<bx*MB_SIZE+MB_SIZE+FOCUS_RANGE; x++) {
			if (y<0 || x <0 || y >= Pic->iHeight || x >= Pic->iWidth) {
				val = FOCUS_BKG_Y;
			} else {
				val = *(Pic->Y + y*Pic->iWidth + x);
			}
			int yy = (y - (by*MB_SIZE-FOCUS_RANGE)) * FOCUS_ZOOM;
			int xx = (x - (bx*MB_SIZE-FOCUS_RANGE)) * FOCUS_ZOOM;
			for (int j=yy; j<yy+FOCUS_ZOOM; j++) {
				for (int i=xx; i<xx+FOCUS_ZOOM; i++) {
					Y[j][i] = val;
				}
			}
		}
	}

	CurrMV_No = -1;
	NextMV();

	Invalidate(FALSE);
}

void CFocusArea::ShowFocusBlock(CDC *pDC)
{
	CMBData* mbd = qmb;
	if (qmb == NULL || FocusBlockX < 0 || FocusBlockY < 0)
		return;

	int x1 = FOCUS_RANGE + MB_SIZE/2;
	int y1 = FOCUS_RANGE + MB_SIZE/2;
	int t = mbd->GetTotalSubMB();
	for (int i=0; i<t; i++) {
		DrawBlock(pDC, x1+mbd->vsb[i].ltx, y1+mbd->vsb[i].lty,
					x1+mbd->vsb[i].rbx, y1+mbd->vsb[i].rby, mbd->vsb[i].mark);
//		DrawArrow(pDC, x1+mbd->vsb[i].cx, y1+mbd->vsb[i].cy,
//					x1+mbd->vsb[i].vcx, y1+mbd->vsb[i].vcy, mbd->vsb[i].mark);
		if (i==CurrMV_No) {
			DrawArrow(pDC, x1+mbd->vsb[i].cx, y1+mbd->vsb[i].cy,
					x1+mbd->vsb[i].cx+(double)CurrMV.mvx/4,
					y1+mbd->vsb[i].cy+(double)CurrMV.mvy/4,
					(bBlink ? QUE : NRM));
		} else {
			DrawArrow(pDC, x1+mbd->vsb[i].cx, y1+mbd->vsb[i].cy,
					x1+mbd->vsb[i].vcx, y1+mbd->vsb[i].vcy, mbd->vsb[i].mark);
		}
	}
// the comments code should be deleted later.
/*	switch(mbd->mode) {
	case B16x16:
		DrawBlock(pDC, x1, y1, x1+16, y1+16, mbd->mv.mark);
		DrawArrow(pDC, x1+8, y1+8, mbd->mv.mvx, mbd->mv.mvy, mbd->mv.mark);
		break;
	case B16x8:
		DrawBlock(pDC, x1, y1, x1+16, y1+8, mbd->mv_top.mark);
		DrawArrow(pDC, x1+8, y1+4, mbd->mv_top.mvx, mbd->mv_top.mvy, mbd->mv_top.mark);
		DrawBlock(pDC, x1, y1+8, x1+16, y1+16, mbd->mv_bottom.mark);
		DrawArrow(pDC, x1+8, y1+12, mbd->mv_bottom.mvx, mbd->mv_bottom.mvy, mbd->mv_bottom.mark);
		break;
	case B8x16:
		DrawBlock(pDC, x1, y1, x1+8, y1+16, mbd->mv_left.mark);
		DrawArrow(pDC, x1+4, y1+8, mbd->mv_left.mvx, mbd->mv_left.mvy, mbd->mv_left.mark);
		DrawBlock(pDC, x1+8, y1, x1+16, y1+16, mbd->mv_right.mark);
		DrawArrow(pDC, x1+12, y1+8, mbd->mv_right.mvx, mbd->mv_right.mvy, mbd->mv_right.mark);
		break;
	case B8x8:
		int i;
		for (i=0; i<4; i++) {
			switch(mbd->mvs[i].mode) {
			case B8:
				DrawBlock(pDC, x1+8*(i%2), y1+8*(i/2), x1+8*(i%2)+8, y1+8*(i/2)+8, mbd->mvs[i].mv.mark);
				DrawArrow(pDC, x1+8*(i%2)+4, y1+8*(i/2)+4, mbd->mvs[i].mv.mvx, mbd->mvs[i].mv.mvy, mbd->mvs[i].mv.mark);
				break;
			case B8x4:
				DrawBlock(pDC, x1+8*(i%2), y1+8*(i/2), x1+8*(i%2)+8, y1+8*(i/2)+4, mbd->mvs[i].mv_top.mark);
				DrawArrow(pDC, x1+8*(i%2)+4, y1+8*(i/2)+2, mbd->mvs[i].mv_top.mvx, mbd->mvs[i].mv_top.mvy, mbd->mvs[i].mv_top.mark);
				DrawBlock(pDC, x1+8*(i%2), y1+8*(i/2)+4, x1+8*(i%2)+8, y1+8*(i/2)+8, mbd->mvs[i].mv_bottom.mark);
				DrawArrow(pDC, x1+8*(i%2)+4, y1+8*(i/2)+6, mbd->mvs[i].mv_bottom.mvx, mbd->mvs[i].mv_bottom.mvy, mbd->mvs[i].mv_bottom.mark);
				break;
			case B4x8:
				DrawBlock(pDC, x1+8*(i%2), y1+8*(i/2), x1+8*(i%2)+4, y1+8*(i/2)+8, mbd->mvs[i].mv_left.mark);
				DrawArrow(pDC, x1+8*(i%2)+2, y1+8*(i/2)+4, mbd->mvs[i].mv_left.mvx, mbd->mvs[i].mv_left.mvy, mbd->mvs[i].mv_left.mark);
				DrawBlock(pDC, x1+8*(i%2)+4, y1+8*(i/2), x1+8*(i%2)+8, y1+8*(i/2)+8, mbd->mvs[i].mv_right.mark);
				DrawArrow(pDC, x1+8*(i%2)+6, y1+8*(i/2)+4, mbd->mvs[i].mv_right.mvx, mbd->mvs[i].mv_right.mvy, mbd->mvs[i].mv_right.mark);
				break;
			case B4x4:
				DrawBlock(pDC, x1+8*(i%2), y1+8*(i/2), x1+8*(i%2)+4, y1+8*(i/2)+4, mbd->mvs[i].mvs[0].mark);
				DrawArrow(pDC, x1+8*(i%2)+2, y1+8*(i/2)+2, mbd->mvs[i].mvs[0].mvx, mbd->mvs[i].mvs[0].mvy, mbd->mvs[i].mvs[0].mark);
				DrawBlock(pDC, x1+8*(i%2)+4, y1+8*(i/2), x1+8*(i%2)+8, y1+8*(i/2)+4, mbd->mvs[i].mvs[1].mark);
				DrawArrow(pDC, x1+8*(i%2)+6, y1+8*(i/2)+2, mbd->mvs[i].mvs[1].mvx, mbd->mvs[i].mvs[1].mvy, mbd->mvs[i].mvs[0].mark);
				DrawBlock(pDC, x1+8*(i%2), y1+8*(i/2)+4, x1+8*(i%2)+4, y1+8*(i/2)+8, mbd->mvs[i].mvs[2].mark);
				DrawArrow(pDC, x1+8*(i%2)+2, y1+8*(i/2)+6, mbd->mvs[i].mvs[2].mvx, mbd->mvs[i].mvs[2].mvy, mbd->mvs[i].mvs[0].mark);
				DrawBlock(pDC, x1+8*(i%2)+4, y1+8*(i/2)+4, x1+8*(i%2)+8, y1+8*(i/2)+8, mbd->mvs[i].mvs[3].mark);
				DrawArrow(pDC, x1+8*(i%2)+6, y1+8*(i/2)+6, mbd->mvs[i].mvs[3].mvx, mbd->mvs[i].mvs[3].mvy, mbd->mvs[i].mvs[0].mark);
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}*/
}

// translate the x pos of the YUV file into the pos of window
int CFocusArea::toWindowX(double x)
{
	int xx;
	CRect rect;
	GetClientRect(&rect);

	xx = (int)( rect.Width() * x * 8 / FOCUS_PIX_SIZE );
	return xx;
}

// translate the y pos of the YUV file into the pos of window
int CFocusArea::toWindowY(double y)
{
	int yy;
	CRect rect;
	GetClientRect(&rect);

	yy = (int)( rect.Height() * y * 8 / FOCUS_PIX_SIZE );
	return yy;
}

// translate the x pos of the window into the pos of YUV file
int CFocusArea::toPictureX(int x)
{
	return 0;
}

// translate the y pos of the window into the pos of YUV file
int CFocusArea::toPictureY(int y)
{
	return 0;
}


void CFocusArea::DrawArrow(CDC *pDC, int cx, int cy, double vx, double vy, int Q)
{
	CPen bluePen(PS_SOLID, 1, RGB(0, 0, 255) );
	CPen redPen(PS_SOLID, 1, RGB(255, 0, 0) );
	CPen greenPen(PS_SOLID, 1, RGB(0, 255, 0) );
	switch( Q ) {
	case NRM: // blue
		pDC->SelectObject(&bluePen);
		break;
	case QUE: // red
		pDC->SelectObject(&redPen);
		break;
	case MDF: // green
		pDC->SelectObject(&greenPen);
		break;
	default:
		return;
	}

	pDC->MoveTo( toWindowX(vx), toWindowY(vy) );
	pDC->LineTo( toWindowX(cx), toWindowY(cy) );

	if (vx-cx == 0 && vy-cy == 0)
		return;

	double a = atan( (double)(-(vy-cy)) / (double)(vx-cx) );
	if (vx-cx < 0) a = 3.1415926 + a;

	double x1 = (cx + 2 * cos(a+0.5));
	double y1 = (cy - 2 * sin(a+0.5));
	pDC->MoveTo( toWindowX(x1), toWindowY(y1) );
	pDC->LineTo( toWindowX(cx), toWindowY(cy) );

	double x2 = (cx + 2 * cos(a-0.5));
	double y2 = (cy - 2 * sin(a-0.5));
	pDC->MoveTo( toWindowX(x2), toWindowY(y2) );
	pDC->LineTo( toWindowX(cx), toWindowY(cy) );
}

// draw a rectangle
void CFocusArea::DrawBlock(CDC *pDC, int x1, int y1, int x2, int y2, int Q)
{
	CPen bluePen(PS_SOLID, 1, RGB(0, 0, 255) );
	CPen redPen(PS_SOLID, 1, RGB(255, 0, 0) );
	CPen greenPen(PS_SOLID, 1, RGB(0, 255, 0) );
	switch( Q ) {
	case NRM: // blue
		pDC->SelectObject(&bluePen);
		break;
	case QUE: // red
		pDC->SelectObject(&redPen);
		break;
	case MDF: // green
		pDC->SelectObject(&greenPen);
		break;
	default:
		return;
	}

	pDC->MoveTo( toWindowX(x1), toWindowY(y1) );
	pDC->LineTo( toWindowX(x2)-1, toWindowY(y1) );
	pDC->LineTo( toWindowX(x2)-1, toWindowY(y2)-1 );
	pDC->LineTo( toWindowX(x1), toWindowY(y2)-1 );
	pDC->LineTo( toWindowX(x1), toWindowY(y1) );
}

void CFocusArea::NextMV()
{
	CMBData* mbd = qmb;

	// get the total MV number firstly
	int totalMV = mbd->GetTotalSubMB();
	// select the next MV with number CurrMV_No.
	if (totalMV > 0) {
		CurrMV_No = (CurrMV_No+1) % totalMV;
		CurrMV.mvx = qmb->vsb[CurrMV_No].mv->mvx;
		CurrMV.mvy = qmb->vsb[CurrMV_No].mv->mvy;
		CurrMV.mark = qmb->vsb[CurrMV_No].mv->mark;
	//	MVData *mv = mbd->vsb[CurrMV_No].mv;

		int bx = mbd->bx;
		int by = mbd->by;
		pDlg->m_FocusRef.SetSearchArea( &(pDlg->m_Reference), bx, by, 
										&CurrMV, &(mbd->vsb[CurrMV_No]) );
	}

/*	int count = -1;
	switch(mbd->mode) {
	case B16x16:
		count ++;
		if (count == CurrMV_No) pDlg->m_FocusRef.SetSearchArea(&(pDlg->m_Reference), &(mbd->mv), bx, by, 0, 0, 16, 16);
		break;
	case B16x8:
		count ++;
		if (count == CurrMV_No) pDlg->m_FocusRef.SetSearchArea(&(pDlg->m_Reference), &(mbd->mv_top), bx, by, 0, 0, 16, 8);
		count ++;
		if (count == CurrMV_No) pDlg->m_FocusRef.SetSearchArea(&(pDlg->m_Reference), &(mbd->mv_bottom), bx, by, 0, 8, 16, 8);
		break;
	case B8x16:
		count ++;
		if (count == CurrMV_No) pDlg->m_FocusRef.SetSearchArea(&(pDlg->m_Reference), &(mbd->mv_left), bx, by, 0, 0, 8, 16);
		count ++;
		if (count == CurrMV_No) pDlg->m_FocusRef.SetSearchArea(&(pDlg->m_Reference), &(mbd->mv_right), bx, by, 8, 0, 8, 16);
		break;
	case B8x8:
		int i;
		for (i=0; i<4; i++) {
			switch(mbd->mvs[i].mode) {
			case B8:
				count ++;
				if (count == CurrMV_No) pDlg->m_FocusRef.SetSearchArea(&(pDlg->m_Reference), &(mbd->mvs[i].mv), bx, by, 8*(i%2), 8*(i/2), 8, 8);
				break;
			case B8x4:
				count ++;
				if (count == CurrMV_No) pDlg->m_FocusRef.SetSearchArea(&(pDlg->m_Reference), &(mbd->mvs[i].mv_top), bx, by, 8*(i%2), 8*(i/2), 8, 4);
				count ++;
				if (count == CurrMV_No) pDlg->m_FocusRef.SetSearchArea(&(pDlg->m_Reference), &(mbd->mvs[i].mv_bottom), bx, by, 8*(i%2), 8*(i/2)+4, 8, 4);
				break;
			case B4x8:
				count ++;
				if (count == CurrMV_No) pDlg->m_FocusRef.SetSearchArea(&(pDlg->m_Reference), &(mbd->mvs[i].mv_left), bx, by, 8*(i%2), 8*(i/2), 4, 8);
				count ++;
				if (count == CurrMV_No) pDlg->m_FocusRef.SetSearchArea(&(pDlg->m_Reference), &(mbd->mvs[i].mv_right), bx, by, 8*(i%2)+4, 8*(i/2), 4, 8);
				break;
			case B4x4:
				count ++;
				if (count == CurrMV_No) pDlg->m_FocusRef.SetSearchArea(&(pDlg->m_Reference), &(mbd->mvs[i].mvs[0]), bx, by, 8*(i%2), 8*(i/2), 4, 4);
				count ++;
				if (count == CurrMV_No) pDlg->m_FocusRef.SetSearchArea(&(pDlg->m_Reference), &(mbd->mvs[i].mvs[1]), bx, by, 8*(i%2)+4, 8*(i/2), 4, 4);
				count ++;
				if (count == CurrMV_No) pDlg->m_FocusRef.SetSearchArea(&(pDlg->m_Reference), &(mbd->mvs[i].mvs[2]), bx, by, 8*(i%2), 8*(i/2)+4, 4, 4);
				count ++;
				if (count == CurrMV_No) pDlg->m_FocusRef.SetSearchArea(&(pDlg->m_Reference), &(mbd->mvs[i].mvs[3]), bx, by, 8*(i%2)+4, 8*(i/2)+4, 4, 4);
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
*/
}

MVData * CFocusArea::GetCurrMV()
{
//	return qmb->vsb[CurrMV_No].mv;
	return &CurrMV;
}

// Get the window width & heidht
void CFocusArea::GetClientRectSize()
{
	CRect rect;
	GetClientRect(&rect);
	rw = rect.Width();
	rh = rect.Height();
}

// refresh the background of the window
void CFocusArea::DrawBackground(CDC *pDC)
{
	CRect rect;
	GetClientRect(&rect);
	rw = rect.Width();
	rh = rect.Height();

	HBRUSH hbkgbr;
	hbkgbr= (HBRUSH)GetStockObject(WHITE_BRUSH);
	FillRect(pDC->m_hDC, rect, hbkgbr);
	DeleteObject(hbkgbr);

	return;
}

void CFocusArea::ModifyMV(MVData *new_mv)
{
	if (CurrMV_No == -1) return;

	int ret = MessageBox("你确认要修改该运动向量吗?", "MV修改确认", MB_ICONQUESTION | MB_OKCANCEL);
	if (ret == IDOK) {
		qmb->vsb[CurrMV_No].mv->mvx = new_mv->mvx;
		qmb->vsb[CurrMV_No].mv->mvy = new_mv->mvy;
		qmb->vsb[CurrMV_No].mv->mark = MDF;

		pDlg->m_playback.ModifyMV(FocusBlockX, FocusBlockY, qmb, CurrMV_No);
	}
}
