//---------------------------------------------------------------------------
#ifndef MTOBJECTS1_INCLUDED
#define MTOBJECTS1_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"
//---------------------------------------------------------------------------
static const int objectstype = FOURCC('X','O','B','J');
//---------------------------------------------------------------------------
class MTObject;
class MTModule;

class ObjectType{
public:
	int type;
	char *description;
	virtual MTObject* MTCT create(MTModule *parent,int id,void *param) = 0;
};
//---------------------------------------------------------------------------
#include "MTObject.h"
#include "MTXAudio.h"
#include "MTXDSP.h"
#include "MTXGUI.h"
#include "MTXSkin.h"
//---------------------------------------------------------------------------
struct MTObjectsPreferences{
	bool hexadecimal;
	bool showzeroes;
};

typedef bool (MTCT *ObjectIOFunc)(MTObject *object,char *filename,void *process);
typedef bool (MTCT *ObjectEditFunc)(MTObject *object,MTWindow *window,int flags,MTUser *user);

struct ObjectIO{
	int type;
	ObjectIOFunc func;
	char *filetypes;
	char *description;
};

struct ObjectEdit{
	int type;
	ObjectEditFunc func;
	char *description;
};

class MTObjectsInterface : public MTXInterface{
public:
	MTObjectsInterface();
	bool MTCT init();
	void MTCT uninit();
	void MTCT start();
	void MTCT stop();
	void MTCT processcmdline(void *params);
	void MTCT showusage(void *out);
	int MTCT config(int command,int param);
	virtual MTObject* MTCT newobject(int type,MTModule *parent,int id,void *param = 0,bool locked = false,bool assign = true);
	virtual bool MTCT deleteobject(MTObject *object);
	virtual bool MTCT loadobject(MTObject *object,const char *filename,void *process = 0);
	virtual bool MTCT saveobject(MTObject *object,const char *filename,void *process = 0);
	virtual bool MTCT ownobject(MTObject *object,MTUser *user,bool silent = true);
	virtual void MTCT freeobject(MTObject *object,MTUser *user);
	virtual bool MTCT editobject(MTObject *object,MTWindow *window,int flags = 0);
	virtual void MTCT closeobject(MTObject *object);
	virtual int MTCT getnumtypes();
	virtual int MTCT gettype(int id);
	virtual ObjectType* MTCT getobjecttype(int type);
	virtual ObjectType* MTCT getobjecttype(const char *description);
	virtual int MTCT addobjecttype(ObjectType *type);
	virtual bool MTCT addload(int type,ObjectIOFunc loadfunc,const char *filetypes,const char *description);
	virtual bool MTCT addsave(int type,ObjectIOFunc savefunc,const char *filetypes,const char *description);
	virtual bool MTCT addedit(int type,ObjectEditFunc editfunc,const char *description);
	virtual void MTCT delobjecttype(ObjectType *type);
	virtual void MTCT delload(int type,ObjectIOFunc loadfunc);
	virtual void MTCT delsave(int type,ObjectIOFunc savefunc);
	virtual void MTCT deledit(int type,ObjectEditFunc editfunc);
};
//---------------------------------------------------------------------------
extern MTInterface *mtinterface;
extern MTObjectsInterface *oi;
extern MTSystemInterface *si;
extern MTAudioInterface *ai;
extern MTDSPInterface *dspi;
extern MTGUIInterface *gi;
extern MTObjectsPreferences objectsprefs;
extern Skin *skin;
extern MTResources *res;
extern MTWindow *monitor;
extern MTHash *objecttype;
extern MTArray *load,*save,*edit;
extern MTLock *objectlock;
extern WaveOutput *output;
#if MTLEVEL >= MTL_PROFESSIONAL
extern bool smpsupport;
#endif
//---------------------------------------------------------------------------
#endif
