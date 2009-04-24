// FocusRef.cpp : implementation file
//

#include "stdafx.h"
#include "MVanalyzer.h"
#include "MVReference.h"
#include "FocusRef.h"
#include "MVAnalyzerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFocusRef

CFocusRef::CFocusRef()
{
	bHave = FALSE;
	SMBX = SMBY = 0;
	CurrCandidate = -1;
	BmpInfo=(BITMAPINFO*)new char [sizeof(BITMAPINFO)+sizeof(RGBQUAD)*256];
}

CFocusRef::~CFocusRef()
{
	delete BmpInfo;
}


BEGIN_MESSAGE_MAP(CFocusRef, CStatic)
	//{{AFX_MSG_MAP(CFocusRef)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFocusRef message handlers

void CFocusRef::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	ShowFocusArea(&dc);
}

void CFocusRef::ShowFocusArea(CDC *pDC)
{
	GetClientRectSize();
	CDC MemDC; CDC *pMemDC; pMemDC = &MemDC; MemDC.CreateCompatibleDC(pDC);  
	CBitmap bmp; bmp.CreateCompatibleBitmap(pDC,rw,rh); MemDC.SelectObject(&bmp);

	DrawBackground(pMemDC);

	if (bHave == FALSE)
		return;

	int i;         
	HANDLE hMem;
	BYTE *lpBuf;

	int BPP = 8;

	BmpInfo->bmiHeader.biSize=sizeof (BITMAPINFOHEADER);
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
	
	int y, x;
	for (y=0; y<FOCUS_PIX_SIZE; y++)
		for (x=0; x<FOCUS_PIX_SIZE; x++)
			tempY[y][x] = 255 - (255-Y[y][x]) / 2;

	int ys = (int)(CurrVSB.vlty*8) - SMBY*MB_SIZE*FOCUS_ZOOM + (FOCUS_RANGE+MB_SIZE/2)*FOCUS_ZOOM;
	int ye = (int)(CurrVSB.vrby*8) - SMBY*MB_SIZE*FOCUS_ZOOM + (FOCUS_RANGE+MB_SIZE/2)*FOCUS_ZOOM;
	int xs = (int)(CurrVSB.vltx*8) - SMBX*MB_SIZE*FOCUS_ZOOM + (FOCUS_RANGE+MB_SIZE/2)*FOCUS_ZOOM;
	int xe = (int)(CurrVSB.vrbx*8) - SMBX*MB_SIZE*FOCUS_ZOOM + (FOCUS_RANGE+MB_SIZE/2)*FOCUS_ZOOM;
	for (y=ys; y<ye; y++)
		for (x=xs; x<xe; x++)
			tempY[y][x] = Y[y][x];

    //将倒置的图象放正 
	//Make the inverse image normal
	for(i=0;i<FOCUS_PIX_SIZE;i++) {
		memcpy(lpBuf+FOCUS_PIX_SIZE*(FOCUS_PIX_SIZE-1-i), tempY[i], FOCUS_PIX_SIZE);
	}

	pMemDC->SetStretchBltMode(STRETCH_DELETESCANS);
	StretchDIBits(pMemDC->m_hDC,0,0,rw,rh,0,0,FOCUS_PIX_SIZE,FOCUS_PIX_SIZE,
				  lpBuf, BmpInfo, DIB_RGB_COLORS, SRCCOPY);  

	GlobalUnlock(hMem);
	GlobalFree(hMem);

	pDC->BitBlt(0, 0, rw, rh, pMemDC, 0, 0, SRCCOPY);
	MemDC.DeleteDC();  
	// Do not call CWnd::OnPaint() for painting messages  
}

#define MAX(a, b) ( (a)>(b) ? (a) : (b) )
void CFocusRef::SetSearchArea(CMVReference *p, int bx, int by, MVData* mvd, MVSubMB* vsb)
{
	FocusBlockX = bx; FocusBlockY = by;
	Pic = p;

	CurrMV.mvx = mvd->mvx; CurrMV.mvy = mvd->mvy; CurrMV.mark = mvd->mark;
	CurrVSB.cx = vsb->cx; CurrVSB.cy = vsb->cy;
	CurrVSB.ltx = vsb->ltx; CurrVSB.lty = vsb->lty;
	CurrVSB.rbx = vsb->rbx; CurrVSB.rby = vsb->rby;
	CurrVSB.vcx = vsb->vcx; CurrVSB.vcy = vsb->vcy;
	CurrVSB.vltx = vsb->vltx; CurrVSB.vlty = vsb->vlty;
	CurrVSB.vrbx = vsb->vrbx; CurrVSB.vrby = vsb->vrby;
	CurrVSB.w =vsb->w; CurrVSB.h = vsb->h; CurrVSB.mark = vsb->mark;
	CurrVSB.mv = &CurrMV;

	SMBX = SMBY = 0;
	if (CurrVSB.vrbx > FOCUS_RANGE+MB_SIZE/2) {
		SMBX = (int)(CurrVSB.vrbx - (FOCUS_RANGE+MB_SIZE/2) + MB_SIZE) / MB_SIZE;
	} else if (CurrVSB.vltx <= -(FOCUS_RANGE+MB_SIZE/2)) {
		SMBX = (int)(CurrVSB.vltx + (FOCUS_RANGE+MB_SIZE/2) - MB_SIZE) / MB_SIZE;
	}
	if (CurrVSB.vrby > FOCUS_RANGE+MB_SIZE/2) {
		SMBY = (int)(CurrVSB.vrby - (FOCUS_RANGE+MB_SIZE/2) + MB_SIZE) / MB_SIZE;
	} else if (CurrVSB.vlty <= -(FOCUS_RANGE+MB_SIZE/2)) {
		SMBY = (int)(CurrVSB.vlty + (FOCUS_RANGE+MB_SIZE/2) - MB_SIZE) / MB_SIZE;
	}

	SetRefArea();
	GetCandidates();
	CurrCandidate = 8;
	NextCandidate();

	bHave = TRUE;
}

void CFocusRef::SetRefArea(void)
{
	int x, y, i, j, yy, xx;
	unsigned char val;

	for (y=(FocusBlockY+SMBY)*MB_SIZE-FOCUS_RANGE; y<(FocusBlockY+SMBY)*MB_SIZE+MB_SIZE+FOCUS_RANGE; y++) {
		for (x=(FocusBlockX+SMBX)*MB_SIZE-FOCUS_RANGE; x<(FocusBlockX+SMBX)*MB_SIZE+MB_SIZE+FOCUS_RANGE; x++) {
			if (y<0 || x <0 || y >= Pic->iHeight || x >= Pic->iWidth) {
				val = FOCUS_BKG_Y;
			} else {
				val = *(Pic->Y + y*Pic->iWidth + x);
			}
			yy = (y - ((FocusBlockY+SMBY)*MB_SIZE-FOCUS_RANGE)) * FOCUS_ZOOM;
			xx = (x - ((FocusBlockX+SMBX)*MB_SIZE-FOCUS_RANGE)) * FOCUS_ZOOM;
			for (j=yy; j<yy+FOCUS_ZOOM; j++) {
				for (i=xx; i<xx+FOCUS_ZOOM; i++) {
					Y[j][i] = val;
				}
			}
		}
	}
	Invalidate(FALSE);

	pDlg->m_Reference.SetRefArea(
		(FocusBlockX+SMBX)*MB_SIZE - FOCUS_RANGE,
		(FocusBlockY+SMBY)*MB_SIZE - FOCUS_RANGE,
		(FocusBlockX+SMBX)*MB_SIZE + FOCUS_RANGE + MB_SIZE - 1,
		(FocusBlockY+SMBY)*MB_SIZE + FOCUS_RANGE + MB_SIZE - 1,
		(FocusBlockX)*MB_SIZE + MB_SIZE/2 + CurrVSB.vltx,
		(FocusBlockX)*MB_SIZE + MB_SIZE/2 + CurrVSB.vrbx,
		(FocusBlockY)*MB_SIZE + MB_SIZE/2 + CurrVSB.vlty,
		(FocusBlockY)*MB_SIZE + MB_SIZE/2 + CurrVSB.vrby
	);
	pDlg->m_Reference.Invalidate(FALSE);

	CalcCurrSAD();
}

void CFocusRef::GetCandidates(void)
{
	int sx[10] = {0, 80, 176, 0, 80, 176, 0, 80, 176, 0};
	int sy[10] = {0, 0, 0, 80, 80, 80, 176, 176, 176, 0};
	int sw[10] = {80, 96, 80, 80, 96, 80, 80, 96, 80, 0};
	int sh[10] = {80, 80, 80, 96, 96, 96, 80, 80, 80, 0};

	for (int i=0; i<9; i++) {
		int minSAD = 1000000;
		for (int m=0; m<sw[i]; m+=2) {
			for (int n=0; n<sh[i]; n+=2) {
				int currSAD = 0;
				for (int x=0; x<CurrVSB.w*8; x+=8) {
					for (int y=0; y<CurrVSB.h*8; y+=8) {
						int t = Y[sy[i]+n+(CurrVSB.lty+8)*8+y][sx[i]+m+(CurrVSB.ltx+8)*8+x] - 
							pDlg->m_FocusArea.Y[128+(CurrVSB.lty+8)*8+y][128+(CurrVSB.ltx+8)*8+x];
						currSAD += (t>0 ? t : -t);
					}
				}
				if (currSAD < minSAD) {
					candidates[i].mvx = ( sx[i] + m - 128 ) / 2 + SMBX * MB_SIZE * 4;
					candidates[i].mvy = ( sy[i] + n - 128 ) / 2 + SMBY * MB_SIZE * 4;
					minSAD = currSAD;
				}
			}
		}
	}
	candidates[9].mvx = CurrMV.mvx;
	candidates[9].mvy = CurrMV.mvy;
	optSAD = CalcCurrSAD();
	char strSAD[16];
	sprintf(strSAD, "%7.3f", optSAD);
	pDlg->m_optSAD.SetWindowText(strSAD);
}

void CFocusRef::NextCandidate()
{
	CurrCandidate = (CurrCandidate + 1) % 10;

	MVData *mv = pDlg->m_FocusArea.GetCurrMV();
	mv->mvx = candidates[CurrCandidate].mvx;
	mv->mvy = candidates[CurrCandidate].mvy;

	CurrVSB.vltx = CurrVSB.vltx + (double)(mv->mvx - CurrMV.mvx)/4;
	CurrVSB.vrbx = CurrVSB.vrbx + (double)(mv->mvx - CurrMV.mvx)/4;
	CurrVSB.vlty = CurrVSB.vlty + (double)(mv->mvy - CurrMV.mvy)/4;
	CurrVSB.vrby = CurrVSB.vrby + (double)(mv->mvy - CurrMV.mvy)/4;
	CurrMV.mvx = mv->mvx;
	CurrMV.mvy = mv->mvy;

	double sad = CalcCurrSAD();
	char strSAD[16];
	sprintf(strSAD, "%7.3f", sad-optSAD);
	pDlg->m_SADdiff.SetWindowText(strSAD);

	SetRefArea();

	Invalidate(FALSE);
}

double CFocusRef::CalcCurrSAD()
{
	int currSAD = 0;
	for (int x=0; x<CurrVSB.w*8; x+=8) {
		for (int y=0; y<CurrVSB.h*8; y+=8) {
			int t = Y[192+(int)((CurrVSB.vlty-SMBY*16)*8)+y][192+(int)((CurrVSB.vltx-SMBX*16)*8)+x] - 
				pDlg->m_FocusArea.Y[192+CurrVSB.lty*8+y][192+CurrVSB.ltx*8+x];
			currSAD += (t>0 ? t : -t);
		}
	}

	if (CurrVSB.w == 0 || CurrVSB.h == 0)
		return 0;
	double sad = (double)currSAD/(CurrVSB.w*CurrVSB.h);
	char strSAD[16];
	sprintf(strSAD, "%7.3f", sad);
	pDlg->m_currSAD.SetWindowText(strSAD);
	return sad;
}

// translate the x pos of the YUV file into the pos of window
int CFocusRef::toWindowX(int x)
{
	int xx;
	CRect rect;
	GetClientRect(&rect);

	xx = ( rect.Width() * x / FOCUS_PIX_SIZE );
	return xx;
}

// translate the y pos of the YUV file into the pos of window
int CFocusRef::toWindowY(int y)
{
	int yy;
	CRect rect;
	GetClientRect(&rect);

	yy = ( rect.Height() * y / FOCUS_PIX_SIZE );
	return yy;
}

// translate the x pos of the window into the pos of YUV file
int CFocusRef::toPictureX(int x)
{
	return 0;
}

// translate the y pos of the window into the pos of YUV file
int CFocusRef::toPictureY(int y)
{
	return 0;
}

void CFocusRef::MoveUp(void)
{
	if (bHave == FALSE) return;

	BOOL bReCalcCandidate = FALSE;
	int temp_y = CurrMV.mvy - 1;
	double temp_vlty = CurrVSB.vlty - 0.25;
	double temp_vrby = CurrVSB.vrby - 0.25;
	int temp_SMBY = SMBY;
	int top_y = -(FOCUS_RANGE+MB_SIZE/2) + SMBY*MB_SIZE;
	int bottom_y = (FOCUS_RANGE+MB_SIZE/2) + SMBY*MB_SIZE;
	if (temp_vrby<=bottom_y && temp_vlty>top_y) {
		temp_SMBY = SMBY;
	} else if (temp_vlty<=top_y) {
		temp_SMBY = SMBY - 1;
		bReCalcCandidate = TRUE;
	} else if (temp_vrby>bottom_y) {
		temp_SMBY = SMBY + 1;
		bReCalcCandidate = TRUE;
	}
	if (FocusBlockY+temp_SMBY >= 0) {
		CurrMV.mvy -= 1;
		MVData *mv = pDlg->m_FocusArea.GetCurrMV(); mv->mvy = CurrMV.mvy;
		CurrVSB.vlty -= 0.25;
		CurrVSB.vrby -= 0.25;
		SMBY = temp_SMBY;
	}

	SetRefArea();
	if (bReCalcCandidate) GetCandidates();
	double sad = CalcCurrSAD();
	char strSAD[16];
	sprintf(strSAD, "%7.3f", sad-optSAD);
	pDlg->m_SADdiff.SetWindowText(strSAD);

	pDlg->m_FocusArea.Invalidate(FALSE);
}

void CFocusRef::MoveDown(void)
{
	if (bHave == FALSE) return;

	BOOL bReCalcCandidate = FALSE;
	int temp_y = CurrMV.mvy + 1;
	double temp_vlty = CurrVSB.vlty + 0.25;
	double temp_vrby = CurrVSB.vrby + 0.25;
	int temp_SMBY = SMBY;
	int top_y = -(FOCUS_RANGE+MB_SIZE/2) + SMBY*MB_SIZE;
	int bottom_y = (FOCUS_RANGE+MB_SIZE/2) + SMBY*MB_SIZE;
	if (temp_vrby<=bottom_y && temp_vlty>top_y) {
		temp_SMBY = SMBY;
	} else if (temp_vlty<=top_y) {
		temp_SMBY = SMBY - 1;
		bReCalcCandidate = TRUE;
	} else if (temp_vrby>bottom_y) {
		temp_SMBY = SMBY + 1;
		bReCalcCandidate = TRUE;
	}
	if (FocusBlockY+temp_SMBY < pDlg->m_playback.iHeight/MB_SIZE) {
		CurrMV.mvy += 1;
		MVData *mv = pDlg->m_FocusArea.GetCurrMV(); mv->mvy = CurrMV.mvy;
		CurrVSB.vlty += 0.25;
		CurrVSB.vrby += 0.25;
		SMBY = temp_SMBY;
	}

	SetRefArea();
	if (bReCalcCandidate) GetCandidates();
	double sad = CalcCurrSAD();
	char strSAD[16];
	sprintf(strSAD, "%7.3f", sad-optSAD);
	pDlg->m_SADdiff.SetWindowText(strSAD);

	pDlg->m_FocusArea.Invalidate(FALSE);
}

void CFocusRef::MoveLeft(void)
{
	if (bHave == FALSE) return;

	BOOL bReCalcCandidate = FALSE;
	double temp_x = CurrMV.mvx - 1;
	double temp_vltx = CurrVSB.vltx - 0.25;
	double temp_vrbx = CurrVSB.vrbx - 0.25;
	int temp_SMBX = SMBX;
	int left_x = -(FOCUS_RANGE+MB_SIZE/2) + SMBX*MB_SIZE;
	int right_x = (FOCUS_RANGE+MB_SIZE/2) + SMBX*MB_SIZE;
	if (temp_vrbx<=right_x && temp_vltx>left_x) {
		temp_SMBX = SMBX;
	} else if (temp_vltx<=left_x) {
		temp_SMBX = SMBX - 1;
		bReCalcCandidate = TRUE;
	} else if (temp_vrbx>right_x) {
		temp_SMBX = SMBX + 1;
		bReCalcCandidate = TRUE;
	}
	if (FocusBlockX+temp_SMBX >= 0) {
		CurrMV.mvx -= 1;
		MVData *mv = pDlg->m_FocusArea.GetCurrMV(); mv->mvx = CurrMV.mvx;
		CurrVSB.vltx -= 0.25;
		CurrVSB.vrbx -= 0.25;
		SMBX = temp_SMBX;
	}

	SetRefArea();
	if (bReCalcCandidate) GetCandidates();
	double sad = CalcCurrSAD();
	char strSAD[16];
	sprintf(strSAD, "%7.3f", sad-optSAD);
	pDlg->m_SADdiff.SetWindowText(strSAD);

	pDlg->m_FocusArea.Invalidate(FALSE);
}

void CFocusRef::MoveRight(void)
{
	if (bHave == FALSE) return;

	BOOL bReCalcCandidate = FALSE;
	double temp_x = CurrMV.mvx + 1;
	double temp_vltx = CurrVSB.vltx + 0.25;
	double temp_vrbx = CurrVSB.vrbx + 0.25;
	int temp_SMBX = SMBX;
	int left_x = -(FOCUS_RANGE+MB_SIZE/2) + SMBX*MB_SIZE;
	int right_x = (FOCUS_RANGE+MB_SIZE/2) + SMBX*MB_SIZE;
	if (temp_vrbx<=right_x && temp_vltx>left_x) {
		temp_SMBX = SMBX;
	} else if (temp_vltx<=left_x) {
		temp_SMBX = SMBX - 1;
		bReCalcCandidate = TRUE;
	} else if (temp_vrbx>right_x) {
		temp_SMBX = SMBX + 1;
		bReCalcCandidate = TRUE;
	}
	if (FocusBlockX+temp_SMBX < pDlg->m_playback.iWidth/MB_SIZE) {
		CurrMV.mvx += 1;
		MVData *mv = pDlg->m_FocusArea.GetCurrMV(); mv->mvx = CurrMV.mvx;
		CurrVSB.vltx += 0.25;
		CurrVSB.vrbx += 0.25;
		SMBX = temp_SMBX;
	}
	
	SetRefArea();
	if (bReCalcCandidate) GetCandidates();
	double sad = CalcCurrSAD();
	char strSAD[16];
	sprintf(strSAD, "%7.3f", sad-optSAD);
	pDlg->m_SADdiff.SetWindowText(strSAD);

	pDlg->m_FocusArea.Invalidate(FALSE);
}

// Get the window width & heidht
void CFocusRef::GetClientRectSize()
{
	CRect rect;
	GetClientRect(&rect);
	rw = rect.Width();
	rh = rect.Height();
}

// refresh the background of the window
void CFocusRef::DrawBackground(CDC *pDC)
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

void CFocusRef::ModifyMV()
{
	pDlg->m_FocusArea.ModifyMV(&CurrMV);
}
