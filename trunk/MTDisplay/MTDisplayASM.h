//---------------------------------------------------------------------------
#ifndef MTDISPLAYASM_INCLUDED
#define MTDISPLAYASM_INCLUDED
//---------------------------------------------------------------------------
#include "../Headers/MTXExtension.h"
//---------------------------------------------------------------------------
typedef void (MTACT *ShadeProc)(void *surface,void *mask,int spitch,int mpitch,int w,int h,int bits);
typedef void (MTACT *SShadeProc)(void *surface,void *mask,int spitch,int mpitch,int w,int h,int mx,int my,int mw,int mh,int bits);
typedef void (MTACT *BlendProc)(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits);
typedef void (MTACT *BlendTProc)(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits,void *maskbits,int maskpitch);
typedef void (MTACT *BlendCKProc)(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits,int colorkey);
typedef void (MTACT *BlurProc)(void *tmp,void *mask,int w,int h,int pitch,int amount);
typedef void (MTACT *OpacityProc)(void *mask,int w,int h,int pitch,int amount);
typedef void (MTACT *FillProc)(void *surface,int pitch,int x,int y,int w,int h,int color,int opacity,int bits);
//---------------------------------------------------------------------------
void extractmask(void *source,void *mask,int color,int *pal,unsigned char **buf);
void __stdcall a_putalpha(void *surface,void *mask,int spitch,int mpitch,int w,int h,int bits);
//---------------------------------------------------------------------------
void MTACT a_shade(void *surface,void *mask,int spitch,int mpitch,int w,int h,int bits);
void MTACT a_sshade(void *surface,void *mask,int spitch,int mpitch,int w,int h,int mx,int my,int mw,int mh,int bits);
void MTACT a_blend(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits);
void MTACT a_blendt(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits,void *maskbits,int maskpitch);
void MTACT a_blendck(void *dest,int dpitch,int x,int y,int w,int h,void *src,int spitch,int ox,int oy,int opacity,int bits,int colorkey);
void MTACT a_blur(void *tmp,void *mask,int w,int h,int pitch,int amount);
void MTACT a_opacity(void *mask,int w,int h,int pitch,int amount);
void MTACT a_fill(void *surface,int pitch,int x,int y,int w,int h,int color,int opacity,int bits);
//---------------------------------------------------------------------------
#endif
