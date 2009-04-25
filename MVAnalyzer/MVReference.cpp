// MVReference.cpp : implementation file
//

#include "stdafx.h"
#include "MVanalyzer.h"
#include "MVReference.h"
#include "MVAnalyzerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMVReference

CMVReference::CMVReference()
{
	// need to modify, use main window reference is ok.
	bHaveFile = FALSE;
	m_pFile = NULL;
	m_pFile = new CFile();

	iTotalFrameNumber = 0;
	iCurrFrameNumber = -1;
	iWidth = 0;
	iHeight = 0;

	bColorful = YUV;

	Y = NULL;
	Cb = NULL;
	Cr = NULL;
	RGBbuf = NULL;

	CenterX = CenterY = 0;
	ZoomFactor = 1.0;

	RefXleft = RefXright = RefYtop = RefYbottom = 0;
	RefBlkXleft = RefBlkXright = RefBlkYtop = RefBlkYbottom = 0;

	BmpInfo=(BITMAPINFO*)new char [sizeof(BITMAPINFO)+sizeof(RGBQUAD)*256];
}

CMVReference::~CMVReference()
{
	// need to modify, use main window reference is ok.
	free(Y);
	free(Cb);
	free(Cr);
	free(RGBbuf);

	delete BmpInfo;
	delete m_pFile;
}


BEGIN_MESSAGE_MAP(CMVReference, CStatic)
	//{{AFX_MSG_MAP(CMVReference)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMVReference message handlers

void CMVReference::OnPaint() 
{
	// need to modify, use main window reference is ok.
	if ( bHaveFile == FALSE )
	{
		CStatic::OnPaint();
		return;
	}

	CPaintDC dc(this); // device context for painting
	
	switch (bColorful) {
	case YUV:
		ShowColorImage(&dc);
		break;
	case YY:
		ShowYImage(&dc);
		break;
	default:
		break;
	}
}

void CMVReference::DoDataExchange(CDataExchange* pDX) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CStatic::DoDataExchange(pDX);
}

/////////////////////////////////////////////////////////////////////////////
// CMVReference public interface
// Constructor/Destructor: CMVReference() , ~CMVReference()
// Mode Setting:	SetColorful()
// Zoom In/Out:		ZoomIn() , ZoomOut()
// Drag Related:	DragStart() , Drag()
// Unknown:			SetPathName() , SetYUVSize() // should be deleted
// Playback:		ReStart() , GoToFrame()
//

// set the colorful or luma only display
void CMVReference::SetColorful(int colorful)
{
	bColorful = colorful;
}

// zoom in the picture without change the center position
void CMVReference::ZoomIn()
{
	GetClientRectSize();
	double maxZF = (double)(iWidth * 4) / (double)rw;

	ZoomFactor *= 1.1;
	if (ZoomFactor == maxZF) return;
	if (ZoomFactor > maxZF) { ZoomFactor = maxZF; }

	ReCalulateShowParam();
	
	Invalidate(FALSE);
}

// zoom out the picture without change the center position
void CMVReference::ZoomOut()
{
	ZoomFactor /= 1.1;
	if (ZoomFactor == 1) return;
	if (ZoomFactor < 1) {
		ZoomFactor = 1;
		x_left = 0; x_right = iWidth-1; CenterX = (x_right-x_left)/2;
		y_top = 0; y_bottom = iHeight-1; CenterY = (y_bottom-y_top)/2;
		edge_x = (int)(16 * rw / (x_right-x_left+32)) + 1;
		edge_y = (int)(16 * rh / (y_bottom-y_top+32)) + 1;
	} else {
		ReCalulateShowParam();
	}

	Invalidate(FALSE);
}

// record the drag start position for Drag()
void CMVReference::DragStart()
{
	DragStart_x_left = x_left;
	DragStart_x_right = x_right;
	DragStart_y_top = y_top;
	DragStart_y_bottom = y_bottom;
	DragStart_CenterX = CenterX;
	DragStart_CenterY = CenterY;
}

// drag the yuv picture move in real time
void CMVReference::Drag(int x, int y)
{
	GetClientRectSize();

	int movex, movey;
	BOOL needRefresh = FALSE;

	// calculate the pro rate movement in the original YUV picture
	movex = (DragStart_x_right - DragStart_x_left) * x / (rw-2*edge_x);
	movey = (DragStart_y_bottom- DragStart_y_top) * y / (rh-2*edge_y);

	if (DragStart_x_left-movex>=0 && DragStart_x_right-movex<iWidth) {
		x_left = DragStart_x_left - movex;
		x_right = DragStart_x_right - movex;
		CenterX = DragStart_CenterX - movex;
		needRefresh = TRUE;
	}
	if (DragStart_y_top-movey>=0 && DragStart_y_bottom-movey<iHeight) {
		y_top = DragStart_y_top - movey;
		y_bottom = DragStart_y_bottom - movey;
		CenterY = DragStart_CenterY - movey;
		needRefresh = TRUE;
	}

	if (needRefresh) {
		Invalidate(FALSE);
	}
}

// need to modify, use main window reference is ok.
void CMVReference::SetPathName(char *pathname)
{
//	if (bHaveFile) {
//		m_pFile->Close();
//	}

	sprintf( sPathName, "%s", pathname );

//	bHaveFile = TRUE;
}

// need to modify, use main window reference is ok.
void CMVReference::SetYUVSize(int width, int height)
{
	iWidth = width;
	iHeight = height;
	CenterX = width >> 1;
	CenterY = height >> 1;

	if (Y!=NULL) free(Y);
	if (Cb!=NULL) free(Cb);
	if (Cr!=NULL) free(Cr);
	if (RGBbuf!=NULL) free(RGBbuf);

	if ( (Y =(unsigned char *)malloc(iWidth*iHeight)) == NULL ) 
	{
		//AfxMessageBox("Couldn't allocate memory for RGBbuf\n");
		return;
	}
	if ( (Cb=(unsigned char *)malloc(iWidth*iHeight/4)) == NULL ) 
	{
		//AfxMessageBox("Couldn't allocate memory for RGBbuf\n");
		return;
	}
	if ( (Cr=(unsigned char *)malloc(iWidth*iHeight/4) ) == NULL ) 
	{
		//AfxMessageBox("Couldn't allocate memory for RGBbuf\n");
		return;
	}
	if ( (RGBbuf=(unsigned char *)malloc(iWidth*iHeight*3)) == NULL ) 
	{
		//AfxMessageBox("Couldn't allocate memory for RGBbuf\n");
		return;
	}
}

// ReStart when open the new YUV file, change the yuv setting, etc.
// It will re-calculate some important member variables, re-read
// the YUV file.
int CMVReference::ReStart()
{
	GetClientRectSize();
	iCurrFrameNumber = -1;

//	if ( bHaveFile == FALSE )
//		return 0;
	if ( bHaveFile ) {
		m_pFile->Close();
	}

	if(!m_pFile->Open(sPathName, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone )) 
	{
		//AfxMessageBox("Can't open input file");
		return 0;
	}
	iTotalFrameNumber = m_pFile->GetLength() / (iWidth*iHeight*3/2);
	GetYUVData();

	ReCalulateShowParam();
	CenterX = (x_left + x_right) / 2;
	CenterY = (y_top + y_bottom) / 2;
	
	Invalidate(FALSE);

	bHaveFile = TRUE;

	return iTotalFrameNumber;
}

// Get the YUV data of frame "number", and display it.
void CMVReference::GoToFrame(int number)
{
	if ( bHaveFile == FALSE )
		return;

	iCurrFrameNumber = number;
	if (iCurrFrameNumber >= iTotalFrameNumber-1 || iCurrFrameNumber < 0) {
		iCurrFrameNumber = -1;
		Invalidate(FALSE);
	}

	if (iCurrFrameNumber >=0 && iCurrFrameNumber <= iTotalFrameNumber-1 ) {
		m_pFile->Seek(iCurrFrameNumber*iWidth*iHeight*3/2, CFile::begin);
		// Read the Y Cb Cr data
		GetYUVData();
		Invalidate(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMVReference private functions
// YUV data reader:		GetYUVData()
// Picture display:		ShowColorImage() , ShowYImage()
// Coordinates translate:	toWindowX() , toWindowY() , toPictureX() , toPictureY()
// Parameters:			ReCalulateShowParam()
// Window related:		GetClientRectSize() , DrawBackground()
//

// requrire the file point is valid and correct
void CMVReference::GetYUVData()
{
	if ( m_pFile->Read(Y, iWidth*iHeight) != (unsigned int)(iWidth*iHeight) ) {
		//AfxMessageBox("Get to end of file");
		return;
	}
	if ( m_pFile->Read(Cb, iWidth*iHeight/4) != (unsigned int)(iWidth*iHeight/4) ) {
		//AfxMessageBox("Get to end of file");
		return;
	}
	if ( m_pFile->Read(Cr, iWidth*iHeight/4) != (unsigned int)(iWidth*iHeight/4) ) {
		//AfxMessageBox("Get to end of file");
		return;
	}
}

// display the colorful image
void CMVReference::ShowColorImage(CDC *pDC)
{
	GetClientRectSize();
	CDC MemDC; CDC *pMemDC; pMemDC = &MemDC; MemDC.CreateCompatibleDC(pDC);  
	CBitmap bmp; bmp.CreateCompatibleBitmap(pDC,rw,rh); MemDC.SelectObject(&bmp);

	DrawBackground(pMemDC);
	if (iCurrFrameNumber < 0) {
		pDC->BitBlt(0, 0, rw, rh, pMemDC, 0, 0, SRCCOPY);
		MemDC.DeleteDC();  
		return;
	}

	int BPP = 24;
	BmpInfo->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	BmpInfo->bmiHeader.biWidth=iWidth;
	BmpInfo->bmiHeader.biHeight=iHeight;			// negative means up-to-bottom 
	BmpInfo->bmiHeader.biPlanes=1;
	BmpInfo->bmiHeader.biBitCount=(BPP<8?8:BPP);	// Our raster is byte-aligned
	BmpInfo->bmiHeader.biCompression=BI_RGB;
	BmpInfo->bmiHeader.biSizeImage=0;
	BmpInfo->bmiHeader.biXPelsPerMeter=11811;
	BmpInfo->bmiHeader.biYPelsPerMeter=11811;
	BmpInfo->bmiHeader.biClrUsed=0;
	BmpInfo->bmiHeader.biClrImportant=0;

	// convert the YUV pictuer(Y,Cb,Cr) into RGB picture(RGBbuf)
	ColorSpaceConversions conv;
	conv.YV12_to_RGB24(Y, Cb, Cr, RGBbuf, iWidth, iHeight);

	// draw the picture
	pMemDC->SetStretchBltMode(STRETCH_DELETESCANS);
	StretchDIBits(pMemDC->m_hDC,
					edge_x, edge_y, rw-2*edge_x, rh-2*edge_y,
					x_left, iHeight-y_bottom, x_right-x_left, y_bottom-y_top,
					RGBbuf, BmpInfo, DIB_RGB_COLORS, SRCCOPY);

	DrawRefArea(pMemDC);

	pDC->BitBlt(0, 0, rw, rh, pMemDC, 0, 0, SRCCOPY);
	MemDC.DeleteDC();  
	// Do not call CWnd::OnPaint() for painting messages  
}

// display the gray image
void CMVReference::ShowYImage(CDC *pDC)
{
	GetClientRectSize();
	CDC MemDC; CDC *pMemDC; pMemDC = &MemDC; MemDC.CreateCompatibleDC(pDC);  
	CBitmap bmp; bmp.CreateCompatibleBitmap(pDC,rw,rh); MemDC.SelectObject(&bmp);

	DrawBackground(pMemDC);
	if (iCurrFrameNumber < 0) {
		pDC->BitBlt(0, 0, rw, rh, pMemDC, 0, 0, SRCCOPY);
		MemDC.DeleteDC();  
		return;
	}

	int i, BPP = 8;
	BmpInfo->bmiHeader.biSize=sizeof (BITMAPINFOHEADER);
	BmpInfo->bmiHeader.biWidth=iWidth;
	BmpInfo->bmiHeader.biHeight=iHeight;			// negative means up-to-bottom 
	BmpInfo->bmiHeader.biPlanes=1;
	BmpInfo->bmiHeader.biBitCount=(BPP<8?8:BPP);	// Our raster is byte-aligned
	BmpInfo->bmiHeader.biCompression=BI_RGB;
	BmpInfo->bmiHeader.biSizeImage=0;
	BmpInfo->bmiHeader.biXPelsPerMeter=11811;
	BmpInfo->bmiHeader.biYPelsPerMeter=11811;
	BmpInfo->bmiHeader.biClrUsed=0;
	BmpInfo->bmiHeader.biClrImportant=0;

	// convert the YUV pictuer(Y,Cb,Cr) into Gray picture(RGBbuf)
	HANDLE hloc1 = LocalAlloc(LMEM_ZEROINIT | LMEM_MOVEABLE,(sizeof(RGBQUAD) * 256));
	RGBQUAD *argbq = (RGBQUAD *) LocalLock(hloc1);
	for(i=0; i<256; i++) {
		argbq[i].rgbBlue = argbq[i].rgbGreen = argbq[i].rgbRed = i;
		argbq[i].rgbReserved = 0;
	}
	memcpy(BmpInfo->bmiColors, argbq, sizeof(RGBQUAD) * 256);
	LocalUnlock(hloc1);
	LocalFree(hloc1);

    HANDLE hMem = GlobalAlloc(GHND, iWidth*iHeight);
	BYTE *lpBuf = (BYTE *)GlobalLock(hMem);
	
	//Make the inverse image up-side-down
	int nNum1,nNum2;
	for(i=0; i<iHeight; i++) {
		nNum1 = (iHeight-i-1)*iWidth;
		nNum2 = i*iWidth;
		memcpy(lpBuf+nNum1, Y+nNum2, iWidth);
	}

	// draw the picture
	GetClientRectSize();
	pMemDC->SetStretchBltMode(STRETCH_DELETESCANS);
	StretchDIBits(pMemDC->m_hDC,
					edge_x, edge_y, rw-2*edge_x, rh-2*edge_y,
					x_left, iHeight-y_bottom, x_right-x_left, y_bottom-y_top,
					lpBuf, BmpInfo, DIB_RGB_COLORS, SRCCOPY);

	GlobalUnlock(hMem);
	GlobalFree(hMem);

	DrawRefArea(pMemDC);

	pDC->BitBlt(0, 0, rw, rh, pMemDC, 0, 0, SRCCOPY);
	MemDC.DeleteDC();  
	// Do not call CWnd::OnPaint() for painting messages  
}

// draw the Current Reference Rectangle Area
void CMVReference::DrawRefArea(CDC *pDC)
{
	CPen redPen(PS_SOLID, 1, RGB(255,0, 0) );
	pDC->SelectObject(&redPen);

	pDC->MoveTo(toWindowX(RefXleft), toWindowY(RefYtop));
	pDC->LineTo(toWindowX(RefXright), toWindowY(RefYtop));
	pDC->LineTo(toWindowX(RefXright), toWindowY(RefYbottom));
	pDC->LineTo(toWindowX(RefXleft), toWindowY(RefYbottom));
	pDC->LineTo(toWindowX(RefXleft), toWindowY(RefYtop));

	pDC->MoveTo(toWindowX(RefBlkXleft), toWindowY(RefBlkYtop));
	pDC->LineTo(toWindowX(RefBlkXright), toWindowY(RefBlkYtop));
	pDC->LineTo(toWindowX(RefBlkXright), toWindowY(RefBlkYbottom));
	pDC->LineTo(toWindowX(RefBlkXleft), toWindowY(RefBlkYbottom));
	pDC->LineTo(toWindowX(RefBlkXleft), toWindowY(RefBlkYtop));
}

// set the Current Reference Rectangle Area
void CMVReference::SetRefArea(int x1, int y1, int x2, int y2, double xb1, double xb2, double yb1, double yb2)
{
	RefXleft = x1;
	RefXright = x2;
	RefYtop = y1;
	RefYbottom = y2;

	RefBlkXleft = xb1;
	RefBlkXright = xb2;
	RefBlkYtop = yb1;
	RefBlkYbottom = yb2;
}

// translate the x pos of the YUV file into the pos of window
int CMVReference::toWindowX(double x)
{
	int xx;
	GetClientRectSize();

	xx = (int)( (rw-2*edge_x) * (x-x_left) / (x_right-x_left) ) + edge_x;
	return xx;
}

// translate the y pos of the YUV file into the pos of window
int CMVReference::toWindowY(double y)
{
	int yy;
	GetClientRectSize();

	yy = (int)( (rh-2*edge_y) * (y-y_top) / (y_bottom-y_top) ) + edge_y;
	return yy;
}

// translate the x pos of the window into the pos of YUV file, un-implemented yet.
int CMVReference::toPictureX(int x)
{
	return 0;
}

// translate the y pos of the window into the pos of YUV file, un-implemented yet.
int CMVReference::toPictureY(int y)
{
	return 0;
}

// recalculate the 4 corner positions of YUV picture, and the x, y edges
// when zoom in/out and move by drag the picture.
void CMVReference::ReCalulateShowParam()
{
	GetClientRectSize();

	// calculate the draw area ( 4 corner positions )
	x_left = (int)(CenterX - iWidth / ZoomFactor / 2);
	x_right = (int)(CenterX + iWidth / ZoomFactor / 2);
	if (x_left<0) x_left = 0; if (x_right>iWidth-1) x_right = iWidth-1;

	y_top = (int)(CenterY - iHeight / ZoomFactor / 2);
	y_bottom = (int)(CenterY + iHeight / ZoomFactor / 2);
	if (y_top<0) y_top = 0; if (y_bottom>iHeight-1) y_bottom = iHeight-1;

	// calculate the edge pro rate of 16 pixels in orignal YUV picture
	edge_x = (int)(16 * rw / (x_right-x_left+32)) + 1;
	edge_y = (int)(16 * rh / (y_bottom-y_top+32)) + 1;
}

// Get the window width & heidht
void CMVReference::GetClientRectSize()
{
	CRect rect;
	GetClientRect(&rect);
	rw = rect.Width();
	rh = rect.Height();
}

// refresh the background of the window
void CMVReference::DrawBackground(CDC *pDC)
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
