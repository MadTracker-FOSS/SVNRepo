//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright � 1999-2003 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//---------------------------------------------------------------------------
#ifndef MTXDISPLAY_INCLUDED
#define MTXDISPLAY_INCLUDED

#ifdef __BORLANDC__
static const int displaytype = 'XDIS';
#else
static const int displaytype = 'SIDX';
#endif

#define MTB_DRAW         0x00000001
#define MTB_SCREEN       0x00000002
#define MTB_SKIN         0x00000004
#define MTB_NOINHERIT    0x0000FFFF
#define MTB_KEEPMASK     0x00010000

#define DTXT_RIGHT     1
#define DTXT_CENTER    2
#define DTXT_BOTTOM    4
#define DTXT_VCENTER   8
#define DTXT_PREFIX    16
#define DTXT_MULTILINE 32

enum{
	MTBM_COPY = 0,
	MTBM_PAINT,
	MTBM_INVERT
};
//---------------------------------------------------------------------------
class MTBitmap;
class MTMask;
class MTDisplayDevice;
//---------------------------------------------------------------------------
#include "MTXExtension.h"
#include "MTXControls.h"
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

typedef void (MTCT *MTBitmapChangeProc)(MTBitmap *oldbitmap,MTBitmap *newbitmap,void *param);

class MTBitmap{
public:
	MTDisplayDevice *device;
	int flags;
	int width,height;
	int bitcount;
	MTRect wr;
	MTBitmapChangeProc changeproc;
	void *param;
	bool loaded;
	
	virtual bool MTCT load() = 0;
	virtual void MTCT unload() = 0;
	virtual bool MTCT setsize(int w,int h) = 0;
	virtual void* MTCT open(int wantedtype) = 0;
	virtual void MTCT close(void *o) = 0;
	virtual void MTCT clip(MTRect *rect) = 0;
	virtual void MTCT cliprgn(void *rgn) = 0;
	virtual void MTCT unclip() = 0;
	virtual void* MTCT getclip() = 0;
	virtual bool MTCT openbits(MTRect &rect,void **bits,int *pitch,void **maskbits = 0,int *maskpitch = 0) = 0;
	virtual void MTCT closebits() = 0;
	virtual bool MTCT blt(MTBitmap *dest,int dx,int dy,int w,int h,int ox,int oy,int mode = MTBM_COPY) = 0;
	virtual bool MTCT sblt(MTBitmap *dest,int dx,int dy,int dw,int dh,int ox,int oy,int ow,int oh,int mode = MTBM_COPY) = 0;
	virtual bool MTCT skinblt(int x,int y,int w,int h,MTSkinPart &o,int color = 0xFFFFFF) = 0;
	virtual bool MTCT skinblta(int x,int y,int w,int h,MTSkinPart &o,int nx,int ny,int step,int color = 0xFFFFFF) = 0;
	virtual bool MTCT maskblt(MTMask *mask,int x,int y,int w,int h,MTSkinPart &o) = 0;
	virtual bool MTCT shade(int x,int y,int w,int h,MTMask *mask,int mx,int my) = 0;
	virtual bool MTCT tshade(int x,int y,int w,int h,MTMask *mask,int mx,int my,int mw,int mh) = 0;
	virtual bool MTCT sshade(int x,int y,int w,int h,MTMask *mask,int mx,int my,int mw,int mh) = 0;
	virtual bool MTCT blendblt(MTBitmap *dest,int dx,int dy,int w,int h,int ox,int oy,int opacity) = 0;
	virtual int MTCT translatecolor(int color) = 0;
	virtual void MTCT setpen(int color) = 0;
	virtual void MTCT setbrush(int color) = 0;
	virtual bool MTCT fill(int x,int y,int w,int h,int color,int opacity = 255) = 0;
	virtual bool MTCT fillex(int x,int y,int w,int h,int mode = MTBM_COPY) = 0;
	virtual void MTCT point(int x,int y,int color) = 0;
	virtual void MTCT moveto(int x,int y) = 0;
	virtual void MTCT lineto(int x,int y) = 0;
	virtual void MTCT polygon(const MTPoint *pt,int np) = 0;
	virtual void MTCT polyline(const MTPoint *pt,int np) = 0;
	virtual void MTCT rectangle(int x,int y,int w,int h) = 0;
	virtual void MTCT ellipse(int x,int y,int w,int h) = 0;
	virtual void MTCT settextcolor(int color) = 0;
	virtual void MTCT setfont(void *font) = 0;
	virtual void MTCT drawtext(const char *text,int length,MTRect &rect,int flags) = 0;
	virtual bool MTCT gettextsize(const char *text,int length,MTPoint *size,int maxwidth = -1) = 0;
	virtual int MTCT gettextextent(const char *text,int length,int maxextent) = 0;
	virtual int MTCT gettextheight() = 0;
	virtual int MTCT getcharwidth(char c) = 0;
	virtual int MTCT getbaseline() = 0;
	virtual void MTCT setwindow(MTWinControl *window) = 0;
	virtual void MTCT setmodified(void *w) = 0;
	virtual void MTCT toscreen(MTPoint &p) = 0;
};

class MTMask{
public:
	MTDisplayDevice *device;
	int width;
	int height;
	
	MTMask(int w,int h);
	virtual void MTCT blur(int amount) =  0;
	virtual void MTCT opacity(int amount) = 0;
	virtual void MTCT fill(int x,int y,int w,int h,int amount) = 0;
};

class MTDisplayDevice{
public:
	virtual bool MTCT init(bool fullscreen) = 0;
	virtual void MTCT uninit() = 0;
	virtual MTBitmap* MTCT newbitmap(int flags,int width,int height) = 0;
	virtual MTBitmap* MTCT newresbitmap(int flags,MTResources *res,int id,int colorkey) = 0;
	virtual MTBitmap* MTCT newfilebitmap(int flags,const char *filename,int colorkey) = 0;
	virtual MTBitmap* MTCT newbmpbitmap(int flags,MTBitmap &orig,int colorkey) = 0;
	virtual void MTCT delbitmap(MTBitmap *bitmap) = 0;
	virtual MTMask* MTCT newmask(int w,int h) = 0;
	virtual void MTCT delmask(MTMask *mask) = 0;
	virtual bool MTCT setfullscreen(bool fullscreen) = 0;
	virtual bool MTCT switchto(MTDisplayDevice *newdevice) = 0;
	virtual void MTCT add(MTBitmap *bmp) = 0;
	virtual void MTCT remove(MTBitmap *bmp) = 0;
	virtual void MTCT checkbitmaps() = 0;
	virtual void MTCT setfocus(bool focused) = 0;
	virtual void MTCT sync() = 0;
};

class MTDisplayDeviceManager{
public:
	char *devicename[16];

	virtual MTDisplayDevice* MTCT newdevice(int id) = 0;
	virtual void MTCT deldevice(MTDisplayDevice *device) = 0;
};

class MTDisplayInterface : public MTXInterface{
public:
	bool fullscreen;

	virtual int MTCT getnumdevices() = 0;
	virtual const char* MTCT getdevicename(int id) = 0;
	virtual void MTCT setdevice(int id,bool silent = false) = 0;
	virtual bool MTCT adddevicemanager(MTDisplayDeviceManager *manager) = 0;
	virtual void MTCT deldevicemanager(MTDisplayDeviceManager *manager) = 0;
	virtual MTBitmap* MTCT newbitmap(int flags,int w,int h) = 0;
	virtual MTBitmap* MTCT newresbitmap(int flags,MTResources *res,int resid,int colorkey = -1) = 0;
	virtual MTBitmap* MTCT newfilebitmap(int flags,const char *filename,int colorkey = -1) = 0;
	virtual MTBitmap* MTCT newbmpbitmap(int flags,MTBitmap &orig,int colorkey = -1) = 0;
	virtual void MTCT setskinbitmap(int bmpid,MTBitmap *newskin) = 0;
	virtual MTMask* MTCT newmask(int w,int h) = 0;
	virtual void MTCT delbitmap(MTBitmap *bmp) = 0;
	virtual void MTCT delmask(MTMask *mask) = 0;
	virtual void MTCT adddesktop(MTWinControl *dsk) = 0;
	virtual void MTCT deldesktop(MTWinControl *dsk) = 0;
	virtual MTWinControl* MTCT getdefaultdesktop() = 0;
	virtual void MTCT checkbitmaps() = 0;
	virtual MTBitmap* MTCT getscreen() = 0;
	virtual void MTCT setfocus(bool focused) = 0;
	virtual void MTCT sync() = 0;
};
//---------------------------------------------------------------------------
#endif
