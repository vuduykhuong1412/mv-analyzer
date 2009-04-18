// MBData.cpp: implementation of the CMBData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MVAnalyzer.h"
#include "MBData.h"
#include "defines.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMBData::CMBData()
{

}

CMBData::~CMBData()
{

}

// check if the macroblock sign
// return 0(NRM): normal block, no questionable MV, no modified MV
// return 1(QUE): at least has one questionable MV.
// return 2(MDF): at least has one modified MV, no questionable MV.
int CMBData::GetMBSign(void)
{
	int modified = NRM;

	switch(mode) {
	case B16x16:
		if (mv.mark == QUE) return QUE;
		if (mv.mark == MDF) modified = MDF;
		break;
	case B16x8:
		if (mv_top.mark == QUE || mv_bottom.mark == QUE)
			return QUE;
		if (mv_top.mark == MDF || mv_bottom.mark == MDF)
			modified = MDF;
		break;
	case B8x16:
		if (mv_left.mark == QUE || mv_right.mark == QUE)
			return QUE;
		if (mv_left.mark == MDF || mv_right.mark == MDF)
			modified = MDF;
		break;
	case B8x8:
		for (int i=0; i<4; i++) {
			switch (mvs[i].mode) {
			case B8:
				if (mvs[i].mv.mark == QUE || mvs[i].mv.mark == QUE)
					return QUE;
				if (mvs[i].mv.mark == MDF || mvs[i].mv.mark == MDF)
					modified = MDF;
				break;
			case B8x4:
				if (mvs[i].mv_top.mark == QUE || mvs[i].mv_bottom.mark == QUE)
					return QUE;
				if (mvs[i].mv_top.mark == MDF || mvs[i].mv_bottom.mark == MDF)
					modified = MDF;
				break;
			case B4x8:
				if (mvs[i].mv_left.mark == QUE || mvs[i].mv_right.mark == QUE)
					return QUE;
				if (mvs[i].mv_left.mark == MDF || mvs[i].mv_right.mark == MDF)
					modified = MDF;
				break;
			case B4x4:
				for (int j=0; j<4; j++) {
					if (mvs[i].mvs[j].mark == QUE)
						return QUE;
					if (mvs[i].mvs[j].mark == MDF)
						modified = MDF;
				}
				break;
			}
		}
		break;
	}

	return modified;
}

int CMBData::GetTotalSubMB()
{
	tsb = -1; // totalSubMB

	switch(mode) {
	case B16x16:
		tsb += 1;
		vsb[tsb].w = 16; vsb[tsb].h = 16;
		vsb[tsb].cx = 0; vsb[tsb].cy = 0;
		vsb[tsb].ltx = -8; vsb[tsb].lty = -8;
		vsb[tsb].rbx = 8; vsb[tsb].rby = 8;
		CalulateV( &mv );
		break;
	case B16x8:
		tsb += 1;
		vsb[tsb].w = 16; vsb[tsb].h = 8;
		vsb[tsb].cx = 0; vsb[tsb].cy = -4;
		vsb[tsb].ltx = -8; vsb[tsb].lty = -8;
		vsb[tsb].rbx = 8; vsb[tsb].rby = 0;
		CalulateV( &mv_top );
		tsb += 1;
		vsb[tsb].w = 16; vsb[tsb].h = 8;
		vsb[tsb].cx = 0; vsb[tsb].cy = 4;
		vsb[tsb].ltx = -8; vsb[tsb].lty = 0;
		vsb[tsb].rbx = 8; vsb[tsb].rby = 8;
		CalulateV( &mv_bottom );
		break;
	case B8x16:
		tsb += 1;
		vsb[tsb].w = 8; vsb[tsb].h = 16;
		vsb[tsb].cx = -4; vsb[tsb].cy = 0;
		vsb[tsb].ltx = -8; vsb[tsb].lty = -8;
		vsb[tsb].rbx = 0; vsb[tsb].rby = 8;
		CalulateV( &mv_left );
		tsb += 1;
		vsb[tsb].w = 8; vsb[tsb].h = 16;
		vsb[tsb].cx = 4; vsb[tsb].cy = 0;
		vsb[tsb].ltx = 0; vsb[tsb].lty = -8;
		vsb[tsb].rbx = 8; vsb[tsb].rby = 8;
		CalulateV( &mv_right );
		break;
	case B8x8:
		int i;
		for (i=0; i<4; i++) {
			switch(mvs[i].mode) {
			case B8:
				tsb += 1;
				vsb[tsb].w = 8; vsb[tsb].h = 8;
				vsb[tsb].cx = (-4+(i%2)*8); vsb[tsb].cy = (-4+(i/2)*8);
				vsb[tsb].ltx = (-8+(i%2)*8); vsb[tsb].lty = (-8+(i/2)*8);
				vsb[tsb].rbx = (0+(i%2)*8); vsb[tsb].rby = (0+(i/2)*8);
				CalulateV( &(mvs[i].mv) );
				break;
			case B8x4:
				tsb += 1;
				vsb[tsb].w = 8; vsb[tsb].h = 4;
				vsb[tsb].cx = (-4+(i%2)*8); vsb[tsb].cy = (-6+(i/2)*8);
				vsb[tsb].ltx = (-8+(i%2)*8); vsb[tsb].lty = (-8+(i/2)*8);
				vsb[tsb].rbx = (0+(i%2)*8); vsb[tsb].rby = (-4+(i/2)*8);
				CalulateV( &(mvs[i].mv_top) );
				tsb += 1;
				vsb[tsb].w = 8; vsb[tsb].h = 4;
				vsb[tsb].cx = (-4+(i%2)*8); vsb[tsb].cy = (-2+(i/2)*8);
				vsb[tsb].ltx = (-8+(i%2)*8); vsb[tsb].lty = (-4+(i/2)*8);
				vsb[tsb].rbx = (0+(i%2)*8); vsb[tsb].rby = (0+(i/2)*8);
				CalulateV( &(mvs[i].mv_bottom) );
				break;
			case B4x8:
				tsb += 1;
				vsb[tsb].w = 4; vsb[tsb].h = 8;
				vsb[tsb].cx = (-6+(i%2)*8); vsb[tsb].cy = (-4+(i/2)*8);
				vsb[tsb].ltx = (-8+(i%2)*8); vsb[tsb].lty = (-8+(i/2)*8);
				vsb[tsb].rbx = (-4+(i%2)*8); vsb[tsb].rby = (0+(i/2)*8);
				CalulateV( &(mvs[i].mv_left) );
				tsb += 1;
				vsb[tsb].w = 4; vsb[tsb].h = 8;
				vsb[tsb].cx = (-2+(i%2)*8); vsb[tsb].cy = (-4+(i/2)*8);
				vsb[tsb].ltx = (-4+(i%2)*8); vsb[tsb].lty = (-8+(i/2)*8);
				vsb[tsb].rbx = (0+(i%2)*8); vsb[tsb].rby = (0+(i/2)*8);
				CalulateV( &(mvs[i].mv_right) );
				break;
			case B4x4:
				tsb += 1;
				vsb[tsb].w = 4; vsb[tsb].h = 4;
				vsb[tsb].cx = (-6+(i%2)*8); vsb[tsb].cy = (-6+(i/2)*8);
				vsb[tsb].ltx = (-8+(i%2)*8); vsb[tsb].lty = (-8+(i/2)*8);
				vsb[tsb].rbx = (-4+(i%2)*8); vsb[tsb].rby = (-4+(i/2)*8);
				CalulateV( &(mvs[i].mvs[0]) );
				tsb += 1;
				vsb[tsb].w = 4; vsb[tsb].h = 4;
				vsb[tsb].cx = (-2+(i%2)*8); vsb[tsb].cy = (-6+(i/2)*8);
				vsb[tsb].ltx = (-4+(i%2)*8); vsb[tsb].lty = (-8+(i/2)*8);
				vsb[tsb].rbx = (0+(i%2)*8); vsb[tsb].rby = (-4+(i/2)*8);
				CalulateV( &(mvs[i].mvs[1]) );
				tsb += 1;
				vsb[tsb].w = 4; vsb[tsb].h = 4;
				vsb[tsb].cx = (-6+(i%2)*8); vsb[tsb].cy = (-2+(i/2)*8);
				vsb[tsb].ltx = (-8+(i%2)*8); vsb[tsb].lty = (-4+(i/2)*8);
				vsb[tsb].rbx = (-4+(i%2)*8); vsb[tsb].rby = (0+(i/2)*8);
				CalulateV( &(mvs[i].mvs[2]) );
				tsb += 1;
				vsb[tsb].w = 4; vsb[tsb].h = 4;
				vsb[tsb].cx = (-2+(i%2)*8); vsb[tsb].cy = (-2+(i/2)*8);
				vsb[tsb].ltx = (-4+(i%2)*8); vsb[tsb].lty = (-4+(i/2)*8);
				vsb[tsb].rbx = (0+(i%2)*8); vsb[tsb].rby = (0+(i/2)*8);
				CalulateV( &(mvs[i].mvs[3]) );
				break;
			default:
				break;
			}
		}
		break;
	case I16x16:
		tsb += 1;
		vsb[tsb].w = 16; vsb[tsb].h = 16;
		vsb[tsb].cx = 0; vsb[tsb].cy = 0;
		vsb[tsb].ltx = -8; vsb[tsb].lty = -8;
		vsb[tsb].rbx = 8; vsb[tsb].rby = 8;
		CalulateV( &mv );
		break;
	case I4x4:
		for (i=0; i<16; i++) {
			tsb += 1;
			vsb[tsb].w = 4; vsb[tsb].h = 4;
			vsb[tsb].cx = -6+(i%4)*4; vsb[tsb].cy = -6+(i/4)*4;
			vsb[tsb].ltx = vsb[tsb].cx-2; vsb[tsb].lty = vsb[tsb].cy-2;
			vsb[tsb].rbx = vsb[tsb].cx+2; vsb[tsb].rby = vsb[tsb].cy+2;
			CalulateV( &mv );
		}
		break;
	case BSKIP:
		tsb += 1;
		vsb[tsb].w = 16; vsb[tsb].h = 16;
		vsb[tsb].cx = 0; vsb[tsb].cy = 0;
		vsb[tsb].ltx = -8; vsb[tsb].lty = -8;
		vsb[tsb].rbx = 8; vsb[tsb].rby = 8;
		CalulateV( &mv );
		break;
	default:
		break;
	}

	return (tsb+1);
}

void CMBData::CalulateV(MVData *mv)
{
	double vx = (double)mv->mvx/4;
	double vy = (double)mv->mvy/4;
	vsb[tsb].vcx = (double)vsb[tsb].cx+vx;
	vsb[tsb].vcy = (double)vsb[tsb].cy+vy;
	vsb[tsb].vltx = (double)vsb[tsb].ltx+vx;
	vsb[tsb].vlty = (double)vsb[tsb].lty+vy;
	vsb[tsb].vrbx = (double)vsb[tsb].rbx+vx;
	vsb[tsb].vrby = (double)vsb[tsb].rby+vy;

	vsb[tsb].mark = mv->mark;
	vsb[tsb].mv = mv;
}
