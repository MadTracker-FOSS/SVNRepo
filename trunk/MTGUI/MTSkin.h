//---------------------------------------------------------------------------
#ifndef MTSKIN_INCLUDED
#define MTSKIN_INCLUDED

#include "../Headers/MTXSkin.h"

#include "MTSysControls.h"
#include "MTSign.h"
#include "MTItems.h"
#include "MTLabel.h"
#include "MTTabControl.h"
#include "MTButton.h"
#include "MTCheckBox.h"
//---------------------------------------------------------------------------
class MTSkin{
public:
	int fontwidth;
	int fontheight;

	MTSkin();
	virtual ~MTSkin();

	virtual int MTCT loadfromstream(MTFile *f,int size,int flags);
	virtual int MTCT savefromstream(MTFile *f,int flags);
//	Controls data
	virtual void MTCT initcontrol(MTControl *ctrl);
	virtual void MTCT uninitcontrol(MTControl *ctrl);
	virtual void MTCT resetcontrol(MTControl *ctrl,bool skinchange);
	virtual void MTCT timercontrol(MTControl *ctrl);
//	Controls drawing
	virtual void MTCT drawcontrol(MTControl *ctrl,MTRect &rect,MTBitmap *b,int x,int y,int flags = 0);
	virtual void MTCT drawborder(MTWinControl *ctrl,MTRect &rect,MTBitmap *b,int x,int y);
	virtual void MTCT drawmodalveil(MTWinControl *ctrl,MTRect &rect);
//	Dimensions
	virtual void MTCT updatemetrics();
	virtual MTBitmap* MTCT getbitmap(int id);
	virtual MTSLMetrics* MTCT getslider(int type,int orientation);
	virtual void MTCT calcbounds(int m);
	virtual void MTCT getcontrolsize(int guiid,int id,int &width,int &height);
	virtual void MTCT getwindowborders(int style,MTRect *borders);
	virtual void MTCT getregions(MTControl *ctrl,void **opaque,void **transparent);
	virtual int MTCT getcolor(int id);
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
//	Shadows
	virtual void MTCT setshadows();
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
	static void MTCT skinchange(MTBitmap *oldskin,MTBitmap *newskin,void *param);
};
//---------------------------------------------------------------------------
bool initSkin();
void uninitSkin();
//---------------------------------------------------------------------------
extern MTRect br[8];
//---------------------------------------------------------------------------
#endif
