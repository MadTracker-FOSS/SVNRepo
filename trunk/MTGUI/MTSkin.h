//---------------------------------------------------------------------------
#ifndef MTSKIN_INCLUDED
#define MTSKIN_INCLUDED
//---------------------------------------------------------------------------
class MTControl;
class MTWinControl;
class MTUserList;
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
//---------------------------------------------------------------------------
class MTSkin{
public:
	int fontwidth;
	int fontheight;

	MTSkin();
	virtual ~MTSkin();

	virtual void MTCT loadfromres(MTResources *res);
	virtual void MTCT savetores(MTResources *res);
//	Controls data
	virtual void MTCT initcontrol(MTControl *ctrl);
	virtual void MTCT uninitcontrol(MTControl *ctrl);
	virtual void MTCT resetcontrol(MTControl *ctrl,bool skinchange);
	virtual void MTCT timercontrol(MTControl *ctrl);
//	Controls drawing
	virtual void MTCT drawcontrol(MTControl *ctrl,MTRect &rect,MTBitmap *b,int x,int y,int flags = 0);
	virtual void MTCT drawcontrol(int guiid,int id,MTRect &rect,MTBitmap *b,int x,int y,int flags = 0);
	virtual void MTCT drawborder(MTWinControl *ctrl,MTRect &rect,MTBitmap *b,int x,int y);
	virtual void MTCT drawmodalveil(MTWinControl *ctrl,MTRect &rect);
//	Dimensions
	virtual void MTCT updatemetrics();
	virtual MTBitmap* MTCT getbitmap(int id);
	virtual MTSLMetrics* MTCT getslider(int type,int orientation);
	virtual void MTCT calcbounds(int m);
	virtual void MTCT setshadows(bool enabled);
	virtual void MTCT getcontrolsize(int guiid,int id,int &width,int &height);
	virtual void MTCT getwindowborders(int style,MTRect *borders);
	virtual void MTCT getwindowoffsets(int style,MTRect *borders);
	virtual void MTCT getregions(MTControl *ctrl,void **opaque,void **transparent);
	virtual int MTCT getcolor(int id);
	virtual void* MTCT getfont(int id);
	virtual bool MTCT gettextsize(MTControl *ctrl,const char *text,int length,MTPoint *size,int maxwidth = -1);
	virtual int MTCT gettextextent(MTControl *ctrl,const char *text,int length,int maxextent);
	virtual int MTCT gettextheight(MTControl *ctrl);
	virtual int MTCT getcharwidth(MTControl *ctrl,char c);
//	Miscellaneous drawing
	virtual void MTCT drawitem(MTUserList *list,int i,MTRect &rect,MTBitmap *b,const char *caption,int imageindex,int itemflags,bool editable);
	virtual void MTCT drawchar(unsigned char c,MTBitmap *bmp,int &x,int y,int color);
	virtual void MTCT drawtext(unsigned char *text,MTBitmap *bmp,int &x,int y,int color);
	virtual void MTCT drawdec(int val,bool zeroes,int n,MTBitmap *bmp,int &x,int y,int color);
	virtual void MTCT drawhex(int val,bool zeroes,int n,MTBitmap *bmp,int &x,int y,int color);
	virtual void MTCT drawwindow(MTBitmap *bmp,int x,int y,int w,int h,int style);
	virtual void MTCT drawinnerwindow(MTBitmap *bmp,int x,int y,int w,int h,int style);
	virtual void MTCT drawbutton(MTBitmap *bmp,int x,int y,int w,int h,MTBTMetrics *m);
	virtual void MTCT drawslider(MTBitmap *bmp,int x,int y,int w,int h,MTSLMetrics *m,int min,int max,int pos,bool focused);
	virtual void MTCT drawframe(MTBitmap *bmp,int x,int y,int w,int h,int flags = 0);
	virtual void MTCT drawshadow(MTBitmap *bmp,int x,int y,int w,int h,int style);

private:
	static unsigned char fontmap[256];
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
	MTRect br[8];
	struct SKO{
		char *k;
		int t;
		void *a;
		int s;
		int p;
	} sko[32];
	static void MTCT skinchange(MTBitmap *oldskin,MTBitmap *newskin,void *param);
	void setfonts();
	void delfonts();
	void delshadows();
};
//---------------------------------------------------------------------------
bool initSkin();
void uninitSkin();
//---------------------------------------------------------------------------
#endif
