// MBData.h: interface for the CMBData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MBDATA_H__381714AC_C249_4453_8D0A_A93ADD33C4A6__INCLUDED_)
#define AFX_MBDATA_H__381714AC_C249_4453_8D0A_A93ADD33C4A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum {NRM, QUE, MDF, BKI, SKP};
typedef struct {
	int mvx;	// motion vector x
	int mvy;	// motion vector y
	int mark;	// question MV or not, 0: normal, 1: questionable, 2: modified
} MVData;

enum {B8, B8x4, B4x8, B4x4};
typedef struct {
	int mode;
	MVData mv;
	MVData mv_left, mv_right;
	MVData mv_top, mv_bottom;
	MVData mvs[4];
} SubMBData;

typedef struct {
	int w;
	int h;

	int cx;
	int cy;
	int ltx;
	int lty;
	int rbx;
	int rby;

	double vcx;
	double vcy;
	double vltx;
	double vlty;
	double vrbx;
	double vrby;

	int mark;
	MVData *mv;
} MVSubMB;

enum {B16x16, B16x8, B8x16, B8x8, I16x16, I4x4, BSKIP};
class CMBData  
{
public:
	CMBData();
	virtual ~CMBData();

public:
	int mode;
	int bx, by;
	MVData mv;
	MVData mv_left, mv_right;
	MVData mv_top, mv_bottom;
	SubMBData mvs[4];
	int tsb;
	MVSubMB vsb[16];

	int GetTotalSubMB(void);
	int GetMBSign(void);

private:
	void CalulateV(MVData *mv);
};

#endif // !defined(AFX_MBDATA_H__381714AC_C249_4453_8D0A_A93ADD33C4A6__INCLUDED_)
