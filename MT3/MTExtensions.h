//---------------------------------------------------------------------------
//	$Id$
//---------------------------------------------------------------------------
#ifndef MTEXTENSIONS_INCLUDED
#define MTEXTENSIONS_INCLUDED

#ifdef __BORLANDC__
static const int mt3type = 'XMT3';
#else
static const int mt3type = '3TMX';
#endif

#define MAX_EXTENSIONS 256

#include "../Headers/MTXExtension.h"
#include "../Headers/MTXSystem.h"
#include "../Headers/MTXObjects.h"
#include "../Headers/MTXDisplay.h"
#include "../Headers/MTXGUI.h"
#include "../Headers/MTXControls.h"
#include "../Headers/MTXAudio.h"
#include "../Headers/MTXDSP.h"
//---------------------------------------------------------------------------
struct MTExtension{
	int id;
	MTXInterfaces *i;
	void *library;
	char *filename;
	bool loaded;
	bool system;
	void *start,*end;
};

class MT3Interface : public MTInterface{
public:
	MT3Interface();
	~MT3Interface();
	MTXInterface* MTCT getinterface(int id);
	MTPreferences* MTCT getprefs();
	void* MTCT getcurrentuser();
	int MTCT getnummodules();
	void* MTCT getmodule(int id);
	void MTCT addmodule(void *module);
	void MTCT delmodule(void *module);
	void MTCT setmodule(void *module);
	bool MTCT addchannel();
	void MTCT notify(void *object,int notify,int param);
	bool MTCT editobject(void *object,bool newwindow);
	void* MTCT getconf(char *name,bool user);
	void MTCT releaseconf(void *conf);
	int MTCT addrefreshproc(RefreshProc proc,void *param);
	void MTCT delrefreshproc(int id);
	char* MTCT getextension(void *ptr);
	bool MTCT canclose();
	void* MTCT getconsole();
	int MTCT processinput(const char *input);
private:
	MTFile* console;
};
//---------------------------------------------------------------------------
void loadExtensions();
void unloadExtensions();
bool initExtensions();
bool startExtensions();
void stopExtensions();
void uninitExtensions();
bool initSystem();
void uninitSystem();
//---------------------------------------------------------------------------
extern int next;
extern MTExtension *ext[MAX_EXTENSIONS];
extern MT3Interface *mi;
extern MTSystemInterface *si;
extern MTObjectsInterface *oi;
extern MTDisplayInterface *di;
extern MTGUIInterface *gi;
extern MTAudioInterface *ai;
extern MTDSPInterface *dspi;
extern MTWindow *lastseq,*lastpw,*lastiw,*lastow,*lastaw;
//---------------------------------------------------------------------------
#endif
