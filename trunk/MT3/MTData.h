//---------------------------------------------------------------------------
//	$Id$
//---------------------------------------------------------------------------
#ifndef MTDATA_INCLUDED
#define MTDATA_INCLUDED
//---------------------------------------------------------------------------
#include "MTExtensions.h"
#include "MTXModule.h"
#include "MTXAudio.h"
#include "MTXSystem.h"
//---------------------------------------------------------------------------
struct _MTConf
{
	int lastuse;
	int refcount;
	MTConfigFile *conf;
};
//---------------------------------------------------------------------------
int newmodule();
//---------------------------------------------------------------------------
bool init();
void uninit();
//---------------------------------------------------------------------------
extern MTPreferences prefs;
extern MTModule *module[16];
extern WaveOutput *output;
extern MTUser cuser;
extern MTFile *outmsg;
extern MTHash *confs;
extern bool wantreset;
//---------------------------------------------------------------------------
#endif
