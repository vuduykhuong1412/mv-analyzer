// MVPlayback.cpp : implementation file
//

#include "stdafx.h"
#include "MVanalyzer.h"
#include "MVplayback.h"
#include "MVAnalyzerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Convert.h"

/////////////////////////////////////////////////////////////////////////////
// CMVPlayback

CMVPlayback::CMVPlayback()
	:CStatic()
{
	bHaveFile = FALSE;
	m_pFile = NULL;
	m_pFile = new CFile();

	iTotalFrameNumber = 0;
	iCurrFrameNumber = 0;
	iWidth = 0;
	iHeight = 0;

	bColorful = YUV;
	bShowMV = FALSE;
	bShowGrid = FALSE;
	bShowSign = FALSE;

	Y = NULL;
	Cb = NULL;
	Cr = NULL;
	RGBbuf = NULL;
	QMB = NULL;

	CenterX = CenterY = 0;
	ZoomFactor = 1.0;
	TotalQMB = 0;
	QMB_bx = -1;
	QMB_by = -1;

	BmpInfo=(BITMAPINFO*)new char [sizeof(BITMAPINFO)+sizeof(RGBQUAD)*256];

	bBlink = 1;
	pMVFile = NULL;

	MVScaleFactor = 1.0;
}

CMVPlayback::~CMVPlayback()
{
	if (winID==0) {
		free(Y);
		free(Cb);
		free(Cr);
		free(RGBbuf);
		free(QMB);
		delete pMVFile;
	}
	delete m_pFile;
	delete BmpInfo;
}

BEGIN_MESSAGE_MAP(CMVPlayback, CStatic)
	//{{AFX_MSG_MAP(CMVPlayback)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMVPlayback message handlers

void CMVPlayback::OnPaint() 
{
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
	case LG:
		ShowLGImage(&dc);
		break;
	case ND:
		ShowNDImage(&dc);
		break;
	default:
		break;
	}
}

void CMVPlayback::DoDataExchange(CDataExchange* pDX)
{
	CStatic::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMVPlayback)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CMVReference public interface
// Constructor/Destructor: CMVReference() , ~CMVReference()
// Mode Setting:	SetColorful() , SetShowMV() , SetShowGrid() , SetShowSign()
// Zoom In/Out:		ZoomIn() , ZoomOut()
// Drag Related:	DragStart() , Drag()
// QMB Finding:		PrevQMB() , NextQMB() , SelectQMB()
// Playback:		ReStart() , GoToFrame()
// Unknown:			SetPathName() , SetYUVSize() // should be deleted
//

// set the colorful display
void CMVPlayback::SetColorful(int colorful)
{
	bColorful = colorful;
}

// set the gray display
void CMVPlayback::SetShowMV(BOOL bMV)
{
	bShowMV = bMV;
}

// set the light gray display
void CMVPlayback::SetShowGrid(BOOL grid)
{
	bShowGrid = grid;
}

// set no display
void CMVPlayback::SetShowSign(BOOL sign)
{
	bShowSign = sign;
}

// zoom in the picture without change the center position
void CMVPlayback::ZoomIn()
{
	GetClientRectSize();
	double maxZF = iWidth * 4 / rw;

	ZoomFactor *= 1.1;
	if (ZoomFactor == maxZF) return;
	if (ZoomFactor > maxZF) { ZoomFactor = maxZF; }

	ReCalulateShowParam();
	
	Invalidate(FALSE);
}

// zoom out the picture without change the center position
void CMVPlayback::ZoomOut()
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
void CMVPlayback::DragStart()
{
	DragStart_x_left = x_left;
	DragStart_x_right = x_right;
	DragStart_y_top = y_top;
	DragStart_y_bottom = y_bottom;
	DragStart_CenterX = CenterX;
	DragStart_CenterY = CenterY;
}

// drag the yuv picture move in real time
void CMVPlayback::Drag(int x, int y)
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

void CMVPlayback::PrevQMB()
{
	if (TotalQMB == 0) return;

	int temp_bx = QMB_bx;
	int temp_by = QMB_by;
	CMBData *temp_mbd = QMB + temp_by*iWidth/MB_SIZE + temp_bx;
	for (int by=0; by<iHeight/MB_SIZE; by++) {
		for (int bx=0; bx<iWidth/MB_SIZE; bx++) {
			CMBData* mbd = QMB + by*iWidth/MB_SIZE + bx;
			if (bx==QMB_bx && by==QMB_by) {
				QMB_bx = temp_bx;
				QMB_by = temp_by;
				pDlg->m_FocusArea.SetFocusArea(temp_mbd, QMB_bx, QMB_by);
				return;
			}
			if (mbd->GetMBSign() == QUE) {
				temp_bx = bx;
				temp_by = by;
				temp_mbd = mbd;
			}
		}
	}
}

void CMVPlayback::NextQMB()
{
	if (TotalQMB == 0) return;

	BOOL foundCurr = FALSE;
	for (int by=0; by<iHeight/MB_SIZE; by++) {
		for (int bx=0; bx<iWidth/MB_SIZE; bx++) {
			CMBData* mbd = QMB + by*iWidth/MB_SIZE + bx;
			if (mbd->GetMBSign() == QUE) {
				if (foundCurr == TRUE) {
					QMB_bx = bx;
					QMB_by = by;
					pDlg->m_FocusArea.SetFocusArea(mbd, bx, by);
					return;
				}
			}
			if (bx==QMB_bx && by==QMB_by)
				foundCurr = TRUE;
		}
	}
}

// double click to select the questionable macroblock by hand
void CMVPlayback::SelectQMB(int x, int y) 
{
	if (x<edge_x || x>=rw-edge_x || y<edge_y || y>=rh-edge_y)
		return;

	int bx = (int)toPictureX(x) / MB_SIZE;
	int by = (int)toPictureY(y) / MB_SIZE;
	QMB_bx = bx;
	QMB_by = by;
	CMBData* mbd = QMB + by*iWidth/MB_SIZE + bx;
	pDlg->m_FocusArea.SetFocusArea(mbd, bx, by);
}

// need to modify, use main window reference is ok.
void CMVPlayback::SetPathName(char *pathname)
{
//	if (bHaveFile) {
//		m_pFile->Close();
//		pMVFile->Clear();
//		delete pMVFile;
//	}

	sprintf( sPathName, "%s", pathname );

	// read the MV data file
	int len = strlen(sPathName);
	strcpy(sMVPathName, sPathName);
	sMVPathName[len-1] = 0; sMVPathName[len-2] = 'v'; sMVPathName[len-3] = 'm'; 
//
//	bHaveFile = TRUE;
}

char* CMVPlayback::GetFileName(void)
{
	if (bHaveFile) {
		char *p = sPathName;
		char *filename = sPathName;
		while (*p++ != 0) {
			if (*p == '\\')
				filename = p+1;
		}
		return filename;
	}
	return NULL;
}

// need to modify, use main window reference is ok.
void CMVPlayback::SetYUVSize(int width, int height)
{
	iWidth = width;
	iHeight = height;
	CenterX = width >> 1;
	CenterY = height >> 1;

	if (winID != 0) {
		Y = pDlg->m_playback.Y;
		Cb = pDlg->m_playback.Cb;
		Cr = pDlg->m_playback.Cr;
		RGBbuf = pDlg->m_playback.RGBbuf;
		QMB = pDlg->m_playback.QMB;
	} else {
		if (Y!=NULL) free(Y);
		if (Cb!=NULL) free(Cb);
		if (Cr!=NULL) free(Cr);
		if (RGBbuf!=NULL) free(RGBbuf);
		if (QMB!=NULL) free(QMB);

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
		if ( (QMB=(CMBData *)malloc(((iWidth+8)/MB_SIZE)*((iHeight+8)/MB_SIZE)*sizeof(CMBData))) == NULL ) 
		{
			//AfxMessageBox("Couldn't allocate memory for RGBbuf\n");
			return;
		}
	}
}

// ReStart when open the new YUV file, change the yuv setting, etc.
// It will re-calculate some important member variables, re-read
// the YUV file.
int CMVPlayback::ReStart()
{
	GetClientRectSize();
	iCurrFrameNumber = 0;

//	if ( bHaveFile == FALSE )
//		return 0;
	if (bHaveFile && winID==0) {
		m_pFile->Close();
		pMVFile->Clear();
		delete pMVFile;
	}

	if (winID != 0) {
		m_pFile = pDlg->m_playback.m_pFile;
	} else {
		if(!m_pFile->Open(sPathName, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone )) {
			//AfxMessageBox("Can't open input file");
			return 0;
		}
	}
	iTotalFrameNumber = m_pFile->GetLength() / (iWidth*iHeight*3/2);

	if (winID != 0) {
		pMVFile = pDlg->m_playback.pMVFile;
	} else {
		pMVFile = new TiXmlDocument( sMVPathName );
		pMVFile->LoadFile();
	}

	GetYUVData();

	ReCalulateShowParam();
	CenterX = (x_left + x_right) / 2;
	CenterY = (y_top + y_bottom) / 2;
	FindQuestionableMV();

	Invalidate(FALSE);

	bHaveFile = TRUE;

	return iTotalFrameNumber;
}

// Get the YUV data of frame "number", and display it.
void CMVPlayback::GoToFrame(int number)
{
	if ( bHaveFile == FALSE )
		return;

	iCurrFrameNumber = number;
	if (iCurrFrameNumber >= iTotalFrameNumber || iCurrFrameNumber < 0) {
		iCurrFrameNumber = -1;
	}

	if (iCurrFrameNumber >=0 && iCurrFrameNumber < iTotalFrameNumber ) {
		m_pFile->Seek(iCurrFrameNumber*iWidth*iHeight*3/2, CFile::begin);
		// Read the Y Cb Cr data
		GetYUVData();
		FindQuestionableMV();
		Invalidate(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMVReference private functions
// YUV data reader:		GetYUVData()
// Picture display:		ShowColorImage() , ShowYImage() , ShowLGImage() , ShowNDImage()
// Information display:	ShowGrids() , DrawRectangle() , ShowMVs() , DrawMV() , ShowSigns() , 
//                      DrawBackground() , DrawBlockSign()
// Coordinates translate:	toWindowX() , toWindowY() , toPictureX() , toPictureY()
// Parameters:			ReCalulateShowParam()
// Window related:		GetClientRectSize() , DrawBackground()
// QMB related:			FindQuestionableMV()
//

// requrire the file point is valid and correct
void CMVPlayback::GetYUVData()
{
	if ( m_pFile->Read(Y, iWidth*iHeight) != (unsigned int)(iWidth*iHeight) )
	{
		//AfxMessageBox("Get to end of file");
		return;
	}
	if ( m_pFile->Read(Cb, iWidth*iHeight/4) != (unsigned int)(iWidth*iHeight/4) )
	{
		//AfxMessageBox("Get to end of file");
		return;
	}
	if ( m_pFile->Read(Cr, iWidth*iHeight/4) != (unsigned int)(iWidth*iHeight/4) )
	{
		//AfxMessageBox("Get to end of file");
		return;
	}

	// read the MV info from the file
	TiXmlNode* fileNode = pMVFile->FirstChild( "file" );
	TiXmlNode* node = NULL;
	while( node = fileNode->IterateChildren( "frame", node ) ) {
		TiXmlElement* frm = node->ToElement();
		int frame_no;
		frm->QueryIntAttribute( "no", &frame_no );
		const char *type = frm->Attribute( "type" );
		if (frame_no == iCurrFrameNumber && strcmp(type, "P_SLICE") == 0) {
			pXMLFrame = node;
			ReadMVfromXML(pXMLFrame);
			break;
		}
	}
}

// get motion vectors information from the *.mv file
void CMVPlayback::ReadMVfromXML(TiXmlNode *pf)
{
	int i = 0;
	TiXmlNode* node = NULL;
	TiXmlNode* sub_n = NULL;
	TiXmlNode* temp_n;

	for (int by=0; by<iHeight/MB_SIZE; by++) {
		for (int bx=0; bx<iWidth/MB_SIZE; bx++) {
			CMBData* mbd = QMB + by*iWidth/MB_SIZE + bx;
			mbd->bx = bx; mbd->by = by;
			mbd->mode = BSKIP;
			mbd->mv.mvx = 0; mbd->mv.mvy = 0; mbd->mv.mark = SKP;
		}
	}

	while( node = pf->IterateChildren( "macroblock", node ) ) {
		TiXmlElement* mb = node->ToElement();
		int bx, by;
		mb->QueryIntAttribute( "xx", &bx );
		mb->QueryIntAttribute( "yy", &by );
		CMBData* mbd = QMB + by*iWidth/MB_SIZE + bx;

		const char *str_type = mb->Attribute( "type" );
		mbd->mode = GetMBtype(str_type);
		mbd->bx = bx; mbd->by = by;
		switch (mbd->mode) {
		case B16x16:
			GetMVfromXML(&(mbd->mv), node->FirstChild( "pre_mv" ));
			break;
		case B16x8:
			temp_n = node->FirstChild( "sub_mb" );
			GetMVfromXML(&(mbd->mv_top), temp_n->FirstChild( "pre_mv" ));
			temp_n = temp_n->NextSibling( "sub_mb" );
			GetMVfromXML(&(mbd->mv_bottom), temp_n->FirstChild( "pre_mv" ));
			break;
		case B8x16:
			temp_n = node->FirstChild( "sub_mb" );
			GetMVfromXML(&(mbd->mv_left), temp_n->FirstChild( "pre_mv" ));
			temp_n = temp_n->NextSibling( "sub_mb" );
			GetMVfromXML(&(mbd->mv_right), temp_n->FirstChild( "pre_mv" ));
			break;
		case B8x8:
			i = 0; sub_n = NULL;
			while (sub_n = node->IterateChildren( "sub_mb", sub_n ) ) {
				TiXmlElement* sub_mb = sub_n->ToElement();

				const char *sub_type = sub_mb->Attribute( "subtype" );
				mbd->mvs[i].mode = GetMBtype(sub_type);
				switch (mbd->mvs[i].mode) {
				case B8:
					GetMVfromXML(&(mbd->mvs[i].mv), sub_n->FirstChild( "pre_mv" ));
					break;
				case B8x4:
					temp_n = sub_n->FirstChild( "sub_sub_mb" );
					GetMVfromXML(&(mbd->mvs[i].mv_top), temp_n->FirstChild( "pre_mv" ));
					temp_n = temp_n->NextSibling( "sub_sub_mb" );
					GetMVfromXML(&(mbd->mvs[i].mv_bottom), temp_n->FirstChild( "pre_mv" ));
					break;
				case B4x8:
					temp_n = sub_n->FirstChild( "sub_sub_mb" );
					GetMVfromXML(&(mbd->mvs[i].mv_left), temp_n->FirstChild( "pre_mv" ));
					temp_n = temp_n->NextSibling( "sub_sub_mb" );
					GetMVfromXML(&(mbd->mvs[i].mv_right), temp_n->FirstChild( "pre_mv" ));
					break;
				case B4x4:
					temp_n = sub_n->FirstChild( "sub_sub_mb" );
					GetMVfromXML(&(mbd->mvs[i].mvs[0]), temp_n->FirstChild( "pre_mv" ));
					temp_n = temp_n->NextSibling( "sub_sub_mb" );
					GetMVfromXML(&(mbd->mvs[i].mvs[1]), temp_n->FirstChild( "pre_mv" ));
					temp_n = temp_n->NextSibling( "sub_sub_mb" );
					GetMVfromXML(&(mbd->mvs[i].mvs[2]), temp_n->FirstChild( "pre_mv" ));
					temp_n = temp_n->NextSibling( "sub_sub_mb" );
					GetMVfromXML(&(mbd->mvs[i].mvs[3]), temp_n->FirstChild( "pre_mv" ));
					break;
				default:
					mbd->mvs[i].mv.mvx = 0;
					mbd->mvs[i].mv.mvy = 0;
					mbd->mvs[i].mv.mark = BKI;
					//AfxMessageBox("Unknow Macroblock type", MB_OK);
					break;
				}
				i++;
			}
			break;
		case I16x16:
			mbd->mv.mvx = 0;
			mbd->mv.mvy = 0;
			mbd->mv.mark = BKI;
			break;
		case I4x4:
			for (i=0; i<16; i++) {
				mbd->mvs[i/4].mvs[i%4].mvx = 0;
				mbd->mvs[i/4].mvs[i%4].mvy = 0;
				mbd->mvs[i/4].mvs[i%4].mark = BKI;
			}
			break;
		default:
			mbd->mv.mvx = 0;
			mbd->mv.mvy = 0;
			mbd->mv.mark = BKI;
			AfxMessageBox("Unknow Macroblock type", MB_OK);
			break;
		}
	}
}

int CMVPlayback::GetMBtype(const char *s)
{
	if (strcmp(s, "P16x16") == 0) return B16x16;
	if (strcmp(s, "P16x8") == 0) return B16x8;
	if (strcmp(s, "P8x16") == 0) return B8x16;
	if (strcmp(s, "P8x8") == 0) return B8x8;
	if (strcmp(s, "SMB8x8") == 0) return B8;
	if (strcmp(s, "SMB8x4") == 0) return B8x4;
	if (strcmp(s, "SMB4x8") == 0) return B4x8;
	if (strcmp(s, "SMB4x4") == 0) return B4x4;
	if (strcmp(s, "I16MB") == 0) return I16x16;
	if (strcmp(s, "I4MB") == 0) return I4x4;

	AfxMessageBox("Unknown Block Type", MB_OK);
	return -1;
}

void CMVPlayback::GetMVfromXML(MVData* mv, TiXmlNode *mv_node)
{
	if (mv==NULL) return;

	TiXmlElement* mv_ele = mv_node->ToElement();
	mv_ele->QueryIntAttribute( "vx", &mv->mvx );
	mv_ele->QueryIntAttribute( "vy", &mv->mvy );

	int tvx, tvy;
	if (mv_ele->QueryIntAttribute( "tvx", &tvx ) == TIXML_NO_ATTRIBUTE) {
		mv->mark = NRM;
	} else {
		mv_ele->QueryIntAttribute( "tvy", &tvy );
		mv->mvx = tvx;
		mv->mvy = tvy;
		mv->mark = MDF;
	}
}

void CMVPlayback::ModifyMV(int bx, int by, CMBData* mbd, int mv_no)
{
	TiXmlNode* node = NULL;
	while( node = pXMLFrame->IterateChildren( "macroblock", node ) ) {
		TiXmlElement* mb = node->ToElement();
		int bxx, byy;
		mb->QueryIntAttribute( "xx", &bxx );
		mb->QueryIntAttribute( "yy", &byy );
		if (bxx == bx && byy == by) {
			break;
		}
	}

	TiXmlNode *sub_n, *temp_n;
	int i, no = -1;
	switch(mbd->mode) {
	case B16x16:
		ModifyCurrMV(mbd, ++no, mv_no, node->FirstChild( "pre_mv" ));
		break;
	case B16x8:
		temp_n = node->FirstChild( "sub_mb" );
		ModifyCurrMV(mbd, ++no, mv_no, temp_n->FirstChild( "pre_mv" ));
		temp_n = temp_n->NextSibling( "sub_mb" );
		ModifyCurrMV(mbd, ++no, mv_no, temp_n->FirstChild( "pre_mv" ));
		break;
	case B8x16:
		temp_n = node->FirstChild( "sub_mb" );
		ModifyCurrMV(mbd, ++no, mv_no, temp_n->FirstChild( "pre_mv" ));
		temp_n = temp_n->NextSibling( "sub_mb" );
		ModifyCurrMV(mbd, ++no, mv_no, temp_n->FirstChild( "pre_mv" ));
		break;
	case B8x8:
		i = 0; sub_n = NULL;
		while (sub_n = node->IterateChildren( "sub_mb", sub_n ) ) {
			TiXmlElement* sub_mb = sub_n->ToElement();
			switch (mbd->mvs[i].mode) {
			case B8:
				ModifyCurrMV(mbd, ++no, mv_no, sub_n->FirstChild( "pre_mv" ));
				break;
			case B8x4:
				temp_n = sub_n->FirstChild( "sub_sub_mb" );
				ModifyCurrMV(mbd, ++no, mv_no, temp_n->FirstChild( "pre_mv" ));
				temp_n = temp_n->NextSibling( "sub_sub_mb" );
				ModifyCurrMV(mbd, ++no, mv_no, temp_n->FirstChild( "pre_mv" ));
				break;
			case B4x8:
				temp_n = sub_n->FirstChild( "sub_sub_mb" );
				ModifyCurrMV(mbd, ++no, mv_no, temp_n->FirstChild( "pre_mv" ));
				temp_n = temp_n->NextSibling( "sub_sub_mb" );
				ModifyCurrMV(mbd, ++no, mv_no, temp_n->FirstChild( "pre_mv" ));
				break;
			case B4x4:
				temp_n = sub_n->FirstChild( "sub_sub_mb" );
				ModifyCurrMV(mbd, ++no, mv_no, temp_n->FirstChild( "pre_mv" ));
				temp_n = temp_n->NextSibling( "sub_sub_mb" );
				ModifyCurrMV(mbd, ++no, mv_no, temp_n->FirstChild( "pre_mv" ));
				temp_n = temp_n->NextSibling( "sub_sub_mb" );
				ModifyCurrMV(mbd, ++no, mv_no, temp_n->FirstChild( "pre_mv" ));
				temp_n = temp_n->NextSibling( "sub_sub_mb" );
				ModifyCurrMV(mbd, ++no, mv_no, temp_n->FirstChild( "pre_mv" ));
				break;
			}
		}
		break;
	default:
		break;
	}
	pMVFile->SaveFile();

	Invalidate(FALSE);
}

void CMVPlayback::ModifyCurrMV(CMBData *mbd, int no, int mv_no, TiXmlNode *mv_node)
{
	if (no == mv_no) {
		TiXmlElement* mv_ele = mv_node->ToElement();
		mv_ele->SetAttribute("tvx", mbd->vsb[no].mv->mvx);
		mv_ele->SetAttribute("tvy", mbd->vsb[no].mv->mvy);
	}
}

// display the colorful image
void CMVPlayback::ShowColorImage(CDC *pDC)
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
					x_left, iHeight-y_bottom, x_right - x_left, y_bottom - y_top,
					RGBbuf, BmpInfo, DIB_RGB_COLORS,SRCCOPY);

	ShowGrids(pMemDC); ShowSigns(pMemDC); ShowMVs(pMemDC); ShowFocus(pMemDC);

	pDC->BitBlt(0, 0, rw, rh, pMemDC, 0, 0, SRCCOPY);
	MemDC.DeleteDC();  
	// Do not call CWnd::OnPaint() for painting messages  
}

// display the gray image
void CMVPlayback::ShowYImage(CDC *pDC)
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

	int BPP = 8;
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
	for(int i=0; i<256; i++) {
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

	ShowGrids(pMemDC); ShowSigns(pMemDC); ShowMVs(pMemDC); ShowFocus(pMemDC);

	pDC->BitBlt(0, 0, rw, rh, pMemDC, 0, 0, SRCCOPY);
	MemDC.DeleteDC();  
	// Do not call CWnd::OnPaint() for painting messages  
}

// display the light gray image
void CMVPlayback::ShowLGImage(CDC *pDC)
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

	int BPP = 8;
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
	for(int i=0; i<256; i++) {
		argbq[i].rgbBlue = argbq[i].rgbGreen = argbq[i].rgbRed = i;
		argbq[i].rgbReserved = 0;
	}
	memcpy(BmpInfo->bmiColors, argbq, sizeof(RGBQUAD) * 256);
	LocalUnlock(hloc1);
	LocalFree(hloc1);

    HANDLE hMem = GlobalAlloc(GHND, iWidth*iHeight);
	BYTE *lpBuf = (BYTE *)GlobalLock(hMem);
	
	//Make the inverse image up-side-down
	BYTE *p = lpBuf;
	unsigned char *y;
	for(i=0; i<iHeight; i++) {
		y = Y + ((iHeight-i-1)*iWidth);
		for (int j=0; j<iWidth; j++) {
			*p = 255 - (255-(unsigned char)(*y)) / 3;
			p++; y++;
		}
	}

	// draw the picture
	pMemDC->SetStretchBltMode(STRETCH_DELETESCANS);
	StretchDIBits(pMemDC->m_hDC,
					edge_x, edge_y, rw-2*edge_x, rh-2*edge_y,
					x_left, iHeight-y_bottom, x_right-x_left, y_bottom-y_top,
					lpBuf, BmpInfo, DIB_RGB_COLORS, SRCCOPY);

	GlobalUnlock(hMem);
	GlobalFree(hMem);

	ShowGrids(pMemDC); ShowSigns(pMemDC); ShowMVs(pMemDC); ShowFocus(pMemDC);

	pDC->BitBlt(0, 0, rw, rh, pMemDC, 0, 0, SRCCOPY);
	MemDC.DeleteDC();  
	// Do not call CWnd::OnPaint() for painting messages  
}

// do not display image
void CMVPlayback::ShowNDImage(CDC *pDC)
{
	GetClientRectSize();
	CDC MemDC; CDC *pMemDC; pMemDC = &MemDC; MemDC.CreateCompatibleDC(pDC);  
	CBitmap bmp; bmp.CreateCompatibleBitmap(pDC,rw,rh); MemDC.SelectObject(&bmp);

	DrawBackground(pMemDC);
	ShowGrids(pMemDC); ShowSigns(pMemDC); ShowMVs(pMemDC); ShowFocus(pMemDC);

	pDC->BitBlt(0, 0, rw, rh, pMemDC, 0, 0, SRCCOPY);
	MemDC.DeleteDC();  
	// Do not call CWnd::OnPaint() for painting messages  
}

// show the macroblock grid
// sub macroblock grid will not be display
void CMVPlayback::ShowGrids(CDC *pDC)
{
	if (!bShowGrid) return;

	CPen bluePen(PS_SOLID, 1, RGB(0,0, 255) );
	pDC->SelectObject(&bluePen);

	for (int by=0; by<iHeight/MB_SIZE; by++) {
		for (int bx=0; bx<iWidth/MB_SIZE; bx++) {
			if ( bx*MB_SIZE>x_right || bx*MB_SIZE+MB_SIZE-1<x_left ||
				 by*MB_SIZE>y_bottom || by*MB_SIZE+MB_SIZE-1<y_top) {
				continue;
			}
			int x1 = toWindowX(bx*MB_SIZE);
			int x2 = toWindowX((bx+1)*MB_SIZE);
			int y1 = toWindowY(by*MB_SIZE);
			int y2 = toWindowY((by+1)*MB_SIZE);
			DrawRectangle(pDC, x1, y1, x2, y2);
		}
	}
}

// draw a rectangle
void CMVPlayback::DrawRectangle(CDC *pDC, int x1, int y1, int x2, int y2)
{
	pDC->MoveTo(x1, y1);
	pDC->LineTo(x2, y1);
	pDC->LineTo(x2, y2);
	pDC->LineTo(x1, y2);
	pDC->LineTo(x1, y1);
}

// show the motion vectors of current picture
void CMVPlayback::ShowMVs(CDC *pDC)
{
	if (!bShowMV) return;
	GetClientRectSize();

	for (int by=0; by<iHeight/MB_SIZE; by++) {
		for (int bx=0; bx<iWidth/MB_SIZE; bx++) {
			CMBData* mbd = QMB + by*iWidth/MB_SIZE + bx;
			int t = mbd->GetTotalSubMB();
			for (int i=0; i<t; i++) {
				DrawMV(pDC, bx*MB_SIZE+8+mbd->vsb[i].cx,
							by*MB_SIZE+8+mbd->vsb[i].cy,
							(double)bx*MB_SIZE+8+(mbd->vsb[i].vcx*MVScaleFactor),
							(double)by*MB_SIZE+8+(mbd->vsb[i].vcy*MVScaleFactor),
							mbd->mode);
			}
// the comments code should be deleted later.
/*			switch(mbd->mode) {
			case B16x16:
				DrawMV(pDC, bx*MB_SIZE+8, by*MB_SIZE+8, mbd->mv.mvx, mbd->mv.mvy);
				break;
			case B16x8:
				DrawMV(pDC, bx*MB_SIZE+8, by*MB_SIZE+4, mbd->mv_top.mvx, mbd->mv_top.mvy);
				DrawMV(pDC, bx*MB_SIZE+8, by*MB_SIZE+12, mbd->mv_bottom.mvx, mbd->mv_bottom.mvy);
				break;
			case B8x16:
				DrawMV(pDC, bx*MB_SIZE+4, by*MB_SIZE+8, mbd->mv_left.mvx, mbd->mv_left.mvy);
				DrawMV(pDC, bx*MB_SIZE+12, by*MB_SIZE+8, mbd->mv_right.mvx, mbd->mv_right.mvy);
				break;
			case B8x8:
				int i;
				for (i=0; i<4; i++) {
					switch(mbd->mvs[i].mode) {
					case B8:
						DrawMV(pDC, bx*MB_SIZE+8*(i%2)+4, by*MB_SIZE+8*(i/2)+4, mbd->mvs[i].mv.mvx, mbd->mvs[i].mv.mvy);
						break;
					case B8x4:
						DrawMV(pDC, bx*MB_SIZE+8*(i%2)+4, by*MB_SIZE+8*(i/2)+2, mbd->mvs[i].mv_top.mvx, mbd->mvs[i].mv_top.mvy);
						DrawMV(pDC, bx*MB_SIZE+8*(i%2)+4, by*MB_SIZE+8*(i/2)+6, mbd->mvs[i].mv_bottom.mvx, mbd->mvs[i].mv_bottom.mvy);
						break;
					case B4x8:
						DrawMV(pDC, bx*MB_SIZE+8*(i%2)+2, by*MB_SIZE+8*(i/2)+4, mbd->mvs[i].mv_left.mvx, mbd->mvs[i].mv_left.mvy);
						DrawMV(pDC, bx*MB_SIZE+8*(i%2)+6, by*MB_SIZE+8*(i/2)+4, mbd->mvs[i].mv_right.mvx, mbd->mvs[i].mv_right.mvy);
						break;
					case B4x4:
						DrawMV(pDC, bx*MB_SIZE+8*(i%2)+2, by*MB_SIZE+8*(i/2)+2, mbd->mvs[i].mvs[0].mvx, mbd->mvs[i].mvs[0].mvy);
						DrawMV(pDC, bx*MB_SIZE+8*(i%2)+6, by*MB_SIZE+8*(i/2)+2, mbd->mvs[i].mvs[1].mvx, mbd->mvs[i].mvs[1].mvy);
						DrawMV(pDC, bx*MB_SIZE+8*(i%2)+2, by*MB_SIZE+8*(i/2)+6, mbd->mvs[i].mvs[2].mvx, mbd->mvs[i].mvs[2].mvy);
						DrawMV(pDC, bx*MB_SIZE+8*(i%2)+6, by*MB_SIZE+8*(i/2)+6, mbd->mvs[i].mvs[3].mvx, mbd->mvs[i].mvs[3].mvy);
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
	}
}

// draw motion vector
void CMVPlayback::DrawMV(CDC *pDC, int cx, int cy, double vx, double vy, int mode)
{
	if (cx<x_left || cx>x_right || cy<y_top || cy>y_bottom)
		return;
	int x1 = toWindowX(cx);
	int y1 = toWindowY(cy);
	int x2 = toWindowX(vx);
	int y2 = toWindowY(vy);
	if (y2<0) {
		y2 = 0;
	}

	if (mode == I16x16 || mode == I4x4) {
		CPen bluePen(PS_SOLID, 1, RGB(0, 0, 255) );
		pDC->SelectObject(&bluePen);
		pDC->Rectangle(x1-1, y1-1, x1+1, y1+1);
		return;
	}
	if (mode == BSKIP) {
		CPen greenPen(PS_SOLID, 1, RGB(0, 255, 0) );
		pDC->SelectObject(&greenPen);
		pDC->Rectangle(x1-1, y1-1, x1+1, y1+1);
		return;
	}

	CPen bluePen(PS_SOLID, 1, RGB(0,0, 255) );
	pDC->SelectObject(&bluePen);
	pDC->MoveTo(x2, y2);
	pDC->LineTo(x1, y1);

	CPen redPen(PS_SOLID, 1, RGB(255,0, 0) );
	pDC->SelectObject(&redPen);
	pDC->Rectangle(x1-1, y1-1, x1+1, y1+1);
}

// show the questionable blocks in red, and 
// show the modified blocks in green.
void CMVPlayback::ShowSigns(CDC *pDC)
{
	if (!bShowSign) return;

	for (int by=0; by<iHeight/MB_SIZE; by++) {
		for (int bx=0; bx<iWidth/MB_SIZE; bx++) {
			if ( bx*MB_SIZE>x_right || bx*MB_SIZE+MB_SIZE-1<x_left ||
				 by*MB_SIZE>y_bottom || by*MB_SIZE+MB_SIZE-1<y_top) {
				continue;
			}
			CMBData* mbd = QMB + by*iWidth/MB_SIZE + bx;
			DrawBlockSign(pDC, bx*MB_SIZE, by*MB_SIZE, 
						(bx+1)*MB_SIZE, (by+1)*MB_SIZE, mbd->GetMBSign());
		}
	}
}

// draw the current focus block with blink.
void CMVPlayback::ShowFocus(CDC *pDC)
{
	if (QMB_bx >= 0 && QMB_by >= 0) {
		int x1, x2, y1, y2;
		x1 = toWindowX(QMB_bx*MB_SIZE); x2 = toWindowX((QMB_bx+1)*MB_SIZE);
		y1 = toWindowY(QMB_by*MB_SIZE); y2 = toWindowY((QMB_by+1)*MB_SIZE);
		if (bBlink) { // questionable
			CPen redPen(PS_SOLID, 1, RGB(255,0,0) );
			pDC->SelectObject(&redPen);
			DrawRectangle(pDC, x1+1, y1+1, x2-1, y2-1);
		} else {
			CPen bluePen(PS_SOLID, 1, RGB(0,0,255) );
			pDC->SelectObject(&bluePen);
			DrawRectangle(pDC, x1+1, y1+1, x2-1, y2-1);
		}
	}
}

// draw the block sign of questionable macroblock
// red -- questionable
// green -- modified
void CMVPlayback::DrawBlockSign(CDC *pDC, int xl, int yt, int xr, int yb, int Q)
{
	int x1, x2, y1, y2;

	x1 = toWindowX(xl); x2 = toWindowX(xr);
	y1 = toWindowY(yt); y2 = toWindowY(yb);
	if (Q == QUE) { // questionable
		CPen redPen(PS_SOLID, 1, RGB(255,0,0) );
		pDC->SelectObject(&redPen);
		DrawRectangle(pDC, x1+1, y1+1, x2-1, y2-1);
	}
	if (Q == MDF) { // modified
		CPen greenPen(PS_SOLID, 1, RGB(0,255,0) );
		pDC->SelectObject(&greenPen);
		DrawRectangle(pDC, x1+1, y1+1, x2-1, y2-1);
	}
	if (Q == BKI) { // Block type I
		CPen yellowPen(PS_SOLID, 1, RGB(255,255,0) );
		pDC->SelectObject(&yellowPen);
		DrawRectangle(pDC, x1+1, y1+1, x2-1, y2-1);
	}
}

// translate the x pos of the YUV file into the pos of window
int CMVPlayback::toWindowX(double x)
{
	int xx;
	GetClientRectSize();

	xx = (int)( (rw-2*edge_x) * (x-x_left) / (x_right-x_left) ) + edge_x;
	return xx;
}

// translate the y pos of the YUV file into the pos of window
int CMVPlayback::toWindowY(double y)
{
	int yy;
	GetClientRectSize();

	yy = (int)( (rh-2*edge_y) * (y-y_top) / (y_bottom-y_top) ) + edge_y;
	return yy;
}

// translate the x pos of the window into the pos of YUV file
double CMVPlayback::toPictureX(int x)
{
	double xx;
	GetClientRectSize();

	xx = (double)(( (x_right-x_left) * (x-edge_x) / (rw-2*edge_x) ) + x_left);
	return xx;
}

// translate the y pos of the window into the pos of YUV file
double CMVPlayback::toPictureY(int y)
{
	double yy;
	GetClientRectSize();

	yy = (double)(( (y_bottom-y_top) * (y-edge_y) / (rh-2*edge_y) ) + y_top);
	return yy;
}

// recalculate the 4 corner positions of YUV picture, and the x, y edges
// when zoom in/out and move by drag the picture.
void CMVPlayback::ReCalulateShowParam()
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
void CMVPlayback::GetClientRectSize()
{
	CRect rect;
	GetClientRect(&rect);
	rw = rect.Width();
	rh = rect.Height();
}

// refresh the background of the window
void CMVPlayback::DrawBackground(CDC *pDC)
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

// find all questionable MBs in the current picture.
// all the questionable MBs are stored in QMB[].
// multi-questionable MVs in the same MB is store as one element in QMB[]/
void CMVPlayback::FindQuestionableMV()
{
	TotalQMB = 0;
	BOOL bFirst = TRUE;

	for (int by=0; by<iHeight/MB_SIZE; by++) {
		for (int bx=0; bx<iWidth/MB_SIZE; bx++) {
//			CreateOneQMB(bx, by);
			CMBData* mbd = QMB + by*iWidth/MB_SIZE + bx;
			int t = mbd->GetTotalSubMB();
			for (int i=0; i<t; i++) {
				if ( mbd->vsb[i].mv->mvx > 128 ||
					 mbd->vsb[i].mv->mvx < -128 ||
					 mbd->vsb[i].mv->mvy > 128 ||
					 mbd->vsb[i].mv->mvx < -128 ) {
					mbd->vsb[i].mv->mark = QUE;
					TotalQMB ++;
					if (bFirst == TRUE) {
						QMB_bx = bx; QMB_by = by;
						pDlg->m_FocusArea.SetFocusArea(mbd, bx, by);
						bFirst = FALSE;
					}
				}
			}
		}
	}
}

// create a QMB for test, should be deleted finally.
/*
void CMVPlayback::CreateOneQMB(int bx, int by)
{
	CMBData *mbd;
	mbd = QMB + by*iWidth/MB_SIZE + bx;

	mbd->bx = bx;
	mbd->by = by;
	switch(rand()%10) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		mbd->mode = B16x16;
		mbd->mv.mvx = (int)(rand()%64 - 32);
		mbd->mv.mvy = (int)(rand()%64 - 32);
		MarkIt(&(mbd->mv));
		break;
	case 7:
		mbd->mode = B16x8;
		mbd->mv_top.mvx = (int)(rand()%64 - 32);
		mbd->mv_top.mvy = (int)(rand()%64 - 32);
		MarkIt(&(mbd->mv_top));
		mbd->mv_bottom.mvx = (int)(rand()%64 - 32);
		mbd->mv_bottom.mvy = (int)(rand()%64 - 32);
		MarkIt(&(mbd->mv_bottom));
		break;
	case 8:
		mbd->mode = B8x16;
		mbd->mv_left.mvx = (int)(rand()%64 - 32);
		mbd->mv_left.mvy = (int)(rand()%64 - 32);
		MarkIt(&(mbd->mv_left));
		mbd->mv_right.mvx = (int)(rand()%64 - 32);
		mbd->mv_right.mvy = (int)(rand()%64 - 32);
		MarkIt(&(mbd->mv_right));
		break;
	case 9:
		mbd->mode = B8x8;
		for (int i=0; i<4; i++) {
			switch(rand()%10) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				mbd->mvs[i].mode = B8;
				mbd->mvs[i].mv.mvx = (int)(rand()%64 - 32);
				mbd->mvs[i].mv.mvy = (int)(rand()%64 - 32);
				MarkIt(&(mbd->mvs[i].mv));
				break;
			case 7:
				mbd->mvs[i].mode = B8x4;
				mbd->mvs[i].mv_top.mvx = (int)(rand()%64 - 32);
				mbd->mvs[i].mv_top.mvy = (int)(rand()%64 - 32);
				MarkIt(&(mbd->mvs[i].mv_top));
				mbd->mvs[i].mv_bottom.mvx = (int)(rand()%64 - 32);
				mbd->mvs[i].mv_bottom.mvy = (int)(rand()%64 - 32);
				MarkIt(&(mbd->mvs[i].mv_bottom));
				break;
			case 8:
				mbd->mvs[i].mode = B4x8;
				mbd->mvs[i].mv_left.mvx = (int)(rand()%64 - 32);
				mbd->mvs[i].mv_left.mvy = (int)(rand()%64 - 32);
				MarkIt(&(mbd->mvs[i].mv_left));
				mbd->mvs[i].mv_right.mvx = (int)(rand()%64 - 32);
				mbd->mvs[i].mv_right.mvy = (int)(rand()%64 - 32);
				MarkIt(&(mbd->mvs[i].mv_right));
				break;
			case 9:
				mbd->mvs[i].mode = B4x4;
				for (int j=0; j<4; j++) {
					mbd->mvs[i].mvs[j].mvx = (int)(rand()%64 - 32);
					mbd->mvs[i].mvs[j].mvy = (int)(rand()%64 - 32);
					MarkIt(&(mbd->mvs[i].mvs[j]));
				}
				break;
			}
		}
		break;
	}
}

// should be deleted finally.
void CMVPlayback::MarkIt(MVData *mv)
{
	if (rand()%100 <= 5) {
		if (rand()%100 <= 80)
			mv->mark = QUE; // questionable
		else
			mv->mark = MDF; // modified
	} else {
		mv->mark = NRM; // normal
	}
}
*/

void CMVPlayback::MVScale(double f)
{
	MVScaleFactor = f;

	Invalidate(FALSE);
}
