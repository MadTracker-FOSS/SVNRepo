//---------------------------------------------------------------------------
#ifndef MTINTERFACE_INCLUDED
#define MTINTERFACE_INCLUDED

#include "../Headers/MTXExtension.h"
#include "../Headers/MTXGUI.h"
#include "../Headers/MTXSkin.h"
#include "../Headers/MTXControls.h"
#include "../Headers/MTXModule.h"
//---------------------------------------------------------------------------
struct RefreshStruct{
	int id;
	RefreshProc proc;
	void *param;
};

class MTSplashLogo : public MTCustomWinBehaviours{
public:
	MTSplashLogo(MTCustomWinControl *control);
	void MTCT ondestroy();
	void MTCT ondraw(MTRect &rect);
	bool MTCT onmessage(MTCMessage &msg);
	void MTCT setalpha(int alpha,int interval,int increment);
private:
	int destalpha;
	int interval,increment;
	int timer;
};
//---------------------------------------------------------------------------
bool initInterface();
void uninitInterface();
bool startInterface();
void stopInterface();
void showInterface();
//---------------------------------------------------------------------------
void setmodule(void *module);
//---------------------------------------------------------------------------
extern void *wnd;
extern MTDesktop *mtdsk;
extern MTResources *mtres,*skinres;
extern Skin *skin;
extern MTImageList *sysimages;
extern MTModule *cmodule;
extern MTArray *refreshprocs;
extern bool candesign;
extern int dragx,dragy;
//---------------------------------------------------------------------------
#endif
