//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//---------------------------------------------------------------------------
#ifndef MTXSKIN_INCLUDED
#define MTXSKIN_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"
#include "MTXDisplay.h"
//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#define sk_back 'BACK'
#define sk_font 'FONT'
#define sk_wins 'WINS'
#define sk_btns 'BTNS'
#define sk_tbtn 'TBTN'
#define sk_dtbt 'DTBT'
#define sk_wbtn 'WBTN'
#define sk_dbtn 'DBTN'
#define sk_mbtn 'MBTN'
#define sk_arrw 'ARRW'
#define sk_cbbt 'CBBT'
#define sk_hscr 'HSCR'
#define sk_vscr 'VSCR'
#define sk_hscb 'HSCB'
#define sk_vscb 'VSCB'
#define sk_hdsc 'HDSC'
#define sk_vdsc 'VDSC'
#define sk_stbt 'STBT'
#define sk_patt 'PATT'
#define sk_hsli 'HSLI'
#define sk_vsli 'VSLI'
#define sk_knob 'KNOB'
#define sk_vols 'VOLS'
#define sk_vume 'VUME'
#define sk_anim 'ANIM'
#define sk_icon 'ICON'
#define sk_chkb 'CHKB'
#define sk_radb 'RADB'
#define sk_imag 'IMAG'
#define sk_prog 'PROG'
#define sk_skin 'SKIN'
#define sk_bmid	'BMID'
#else
#define sk_back 'KCAB'
#define sk_font 'TNOF'
#define sk_wins 'SNIW'
#define sk_btns 'SNTB'
#define sk_tbtn 'NTBT'
#define sk_dtbt 'TBTD'
#define sk_wbtn 'NTBW'
#define sk_dbtn 'NTBD'
#define sk_mbtn 'NTBM'
#define sk_arrw 'WRRA'
#define sk_cbbt 'TBBC'
#define sk_hscr 'RCSH'
#define sk_vscr 'RCSV'
#define sk_hscb 'BCSH'
#define sk_vscb 'BCSV'
#define sk_hdsc 'CSDH'
#define sk_vdsc 'CSDV'
#define sk_stbt 'TBTS'
#define sk_patt 'TTAP'
#define sk_hsli 'ILSH'
#define sk_vsli 'ILSV'
#define sk_knob 'BONK'
#define sk_vols 'SLOV'
#define sk_vume 'EMUV'
#define sk_anim 'MINA'
#define sk_icon 'NOCI'
#define sk_chkb 'BKHC'
#define sk_radb 'BDAR'
#define sk_imag 'GAMI'
#define sk_prog 'GORP'
#define sk_skin 'NIKS'
#define sk_bmid	'DIMB'
#endif

enum{
	SC_BACKGROUND = 0xFF000000,
	SC_TEXT_NORMAL,
	SC_TEXT_FOCUSED,
	SC_WINDOW_NORMAL,
	SC_WINDOW_FOCUSED,
	SC_EDIT_BACKGROUND,
	SC_EDIT_NORMAL,
	SC_EDIT_FOCUSED,
	SC_EDIT_SELECTION,
	SC_EDIT_SELECTED,
	SC_BUTTON_NORMAL,
	SC_BUTTON_FOCUSED,
	SC_BUTTON_DISABLED,
	SC_LINK_NORMAL,
	SC_LINK_FOCUSED,
	SC_TAB_NORMAL,
	SC_TAB_FOCUSED,
	SC_CTRL_HL,
	SC_CTRL_L,
	SC_CTRL_BKG,
	SC_CTRL_S,
	SC_CTRL_DS,
	SC_CURSOR,
	SC_PATT_TEXT1,
	SC_PATT_TEXT2,
	SC_PATT_TEXT3,
	SC_PATT_TEXT4,
	SC_PATT_TEXT5,
	SC_PATT_TEXT6,
	SC_PATT_TEXT7,
	SC_PATT_TEXT8,
	SC_PATT_BACK1,
	SC_PATT_BACK2,
	SC_PATT_BACK3,
	SC_PATT_BACK4,
	SC_PATT_BACK5,
	SC_PATT_BACK6,
	SC_PATT_BACK7,
	SC_PATT_BACK8,
	SC_MAX
};

enum{
	SKIN_FIXED = 0x0000,
	SKIN_STRETCH,
	SKIN_TILE,
	SKIN_COLOR
};

#define SKIN_TRANSP 0x01
#define SKIN_ALPHA  0x02

enum{
	SKIN_BUTTON = 0,
	SKIN_PROGR,
	SKIN_ANIM,
	SKIN_CIRCLE,
	SKIN_VUMETER,
	SKIN_XBUTTON,
	SKIN_XPROGR,
	SKIN_XVUMETER
};

enum{
	SKIN_HORIZ = 0,
	SKIN_VERT,
	SKIN_BOTH
};

#define SF_BOLD       0x01
#define SF_ITALIC     0x02
#define SF_UNDERLINED 0x04
#define SF_CENTER     0x10
#define SF_RIGHT      0x20
//---------------------------------------------------------------------------
#ifndef MTGUITypes
#define MTGUITypes
struct MTPoint{
	int x,y;
};

struct MTRect{
	int left,top,right,bottom;
};

struct MTBounds{
	unsigned short x;
	unsigned short y;
	unsigned short w;
	unsigned short h;
};
#endif

#ifndef MTSkinTypes
#define MTSkinTypes
struct MTSkinPart{
	unsigned char mode,flags,bmpid,reserved;
	union{
		MTBounds b;
		int color;
	};
};
#endif

struct MTSQMetrics{
	MTSkinPart a;
	int nx,ny;
	int speed;
};

struct MTCANIMetrics{
	MTSkinPart a;
	MTSkinPart fa;
	int nx;
	int ny;
};

struct MTFontDesc{
	unsigned short flags;
	unsigned short size;
	char name[48];
};

struct MTFNMetrics{
	MTFontDesc caption;
	MTFontDesc text;
	MTFontDesc button;
  MTFontDesc edit;
	unsigned char fontseq[96];
	MTSQMetrics pattfont;
	int colors[SC_MAX-SC_BACKGROUND];
};

struct MTBTMetrics{
	MTSkinPart bkg;
	MTSkinPart tl;
	MTSkinPart t;
	MTSkinPart tr;
	MTSkinPart bl;
	MTSkinPart b;
	MTSkinPart br;
	MTSkinPart l;
	MTSkinPart r;
};

struct MTWNMetrics : public MTBTMetrics{
	MTPoint tlo;
	MTPoint tro;
	MTPoint blo;
	MTPoint bro;
	int to;
	int bo;
	int lo;
	int ro;
	MTRect co;
	MTPoint btno;
};

struct MTBSLM{
	MTSkinPart s;
	MTSkinPart b;
	MTSkinPart fb;
	MTRect sa;
};

struct MTPSLM{
	MTSkinPart s;
	MTSkinPart ps;
	MTSkinPart pm;
	MTSkinPart pe;
	MTRect sa;
};

struct MTCSLM{
	MTSkinPart s;
	MTSkinPart b;
	MTSkinPart fb;
	MTPoint cp;
	int r;
};

struct MTVSLM{
	MTSkinPart s;
	MTSkinPart v;
	MTRect sa;
};

struct MTXBSLM{
	MTBTMetrics s;
	MTSkinPart b;
	MTSkinPart fb;
	MTRect sa;
};

struct MTXPSLM{
	MTBTMetrics s;
	MTSkinPart ps;
	MTSkinPart pm;
	MTSkinPart pe;
	MTRect sa;
};

struct MTXVSLM{
	MTBTMetrics s;
	MTSkinPart v;
	MTRect sa;
};

struct MTSLMetrics{
	int type;
	int orientation;
	union{
		MTBSLM a;
		MTPSLM b;
		MTCANIMetrics c;
		MTCSLM d;
		MTVSLM e;
		MTXBSLM xa;
		MTXPSLM xb;
		MTXVSLM xe;
	};
};

struct MTSkinInfo{
	int colorkey;
	char skinname[128];
	char skinauthor[64];
	char skinemail[64];
	char skinurl[256];
};
/*
struct MTSkin{
	int bmpid[16];
	MTBitmap *skinbmp[16];
	MTMask *mask[8];
	MTRect mr[8][8];
	void *hskfont[4];
	MTSkinPart bkg;
	MTFNMetrics fnm;
	MTWNMetrics wnm[8];
	MTBTMetrics btm[5];
	MTBTMetrics tbtm[4];
	MTBTMetrics dtbtm[4];
	MTSQMetrics wbtns;
	MTSQMetrics dbtns;
	MTSQMetrics mbtns;
	MTSkinPart arm;
	MTSkinPart cbbm;
	MTBTMetrics hscm;
	MTBTMetrics vscm;
	MTBTMetrics hscbm;
	MTBTMetrics vscbm;
	MTBTMetrics hdscm;
	MTBTMetrics vdscm;
	MTSkinPart sbm;
	MTBTMetrics ptm[4];
	MTSLMetrics hslm;
	MTSLMetrics vslm;
	MTSLMetrics knm;
	MTSLMetrics vsm;
	MTSLMetrics vum;
	MTSQMetrics animm[4];
	MTSQMetrics icom;
	MTSkinPart cbm;
	MTSkinPart rbm;
	MTSQMetrics ilm;
	MTSLMetrics prm;
	MTSkinInfo info;
};
*/
//---------------------------------------------------------------------------
#endif
