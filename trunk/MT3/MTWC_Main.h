//---------------------------------------------------------------------------
#ifndef MTWC_MAIN_INCLUDED
#define MTWC_MAIN_INCLUDED
//---------------------------------------------------------------------------
#include "../Headers/MTXWrapper.h"
//---------------------------------------------------------------------------
class MTWC_main : public MTWrapper{
public:
	MTButton* mfile;
	MTButton* medit;
	MTButton* mmodule;
	MTButton* mwindow;
	MTButton* mhelp;

	MTWC_main(MTWindow *w);
	~MTWC_main();
	bool MTCT onmessage(MTCMessage &msg);
private:
//	Your private declarations
	MTMainMenu* menu;
	MTPoint mo;
	MTRect mwo;
	bool moving,triggered;
};
//---------------------------------------------------------------------------
extern MTWC_main *w_main;
//---------------------------------------------------------------------------
#endif
