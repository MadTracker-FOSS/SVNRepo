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
#ifndef MTXOBJECT_INCLUDED
#define MTXOBJECT_INCLUDED

#define MTO_MODULE      0x01000000
#define MTO_PATTERN     0x02000000
#define MTO_INSTRUMENT  0x03000000
#define MTO_OSCILLATOR  0x04000000
#define MTO_AUTOMATION  0x05000000
#define MTO_TRACK       0x06000000
#define MTO_EFFECT      0x07000000
#define MTO_TRACKEFFECT 0x08000000
#define MTO_IDMASK      0x00FFFFFF
#define MTO_TYPEMASK    0x7F000000
#define MTO_DEVELOPMENT 0x80000000

enum{
	MTOL_READ = 1,
	MTOL_WRITE = 2,
	MTOL_LOCK = 3
};

enum{
	MTOA_CANREAD = 1,
	MTOA_CANWRITE = 2,
	MTOA_CANCOPY = 4,
	MTOA_CANDELETE = 8,
	MTOA_CANPLAY = 16,
	MTOA_CANALL = 255
};

enum{
	MTN_NEW = 1,
	MTN_DELETE,
	MTN_MODIFY,
	MTN_LOCK,
	MTN_UNLOCK,
	MTN_RENAME,
	MTN_FREQUENCY = 0x80,
	MTN_TEMPO,
	MTN_PINSTANCEDELETE = 0x100,
	MTN_IINSTANCEDELETE
};

#define MTOM_ADDDELETE  0x10000
#define MTOM_NAME       0x20000
#define MTOM_PARAM      0x40000
#define MTOM_DATA       0x80000
#define MTOM_NOUPDATE   0x100000

#define MTUID_EVERYONE   0x00000000
#define MTUID_REGISTERED 0xFFFFFFFF
//---------------------------------------------------------------------------
struct EnvPoint;
struct MTUser;
struct MTRule;
struct MTAccess;
class MTObject;
class MTModule;
//---------------------------------------------------------------------------
#include "MTXExtension.h"
#include "MTXSystem.h"
//---------------------------------------------------------------------------
struct EnvPoint{
	float x;
	float y;
};

// Users
struct MTUser{
	unsigned int id;
	char name[32];
	char handle[32];
	char group[32];
	char address[128];
	char email[64];
	char url[128];
};

// Access
struct MTRule{
	int userid;
	int access;
};

struct MTAccess{
	int creatorid;
	int caccess;
	int nrules;
	MTRule **rules;
};

typedef int (MTCT *MTObjectEnum)(MTObject *object,void *data);

// MadTracker object
class MTObject{
protected:
	int modified;
public:
	MTAccess access;
	int lockread;
	int lockwrite;
	int modifying;
	MTModule *parent;
	MTUser *owner;
	MTUser *lastowner;
	int objecttype;
	int id;
	int flags;
	char *name;
	
	MTObject(MTModule *parent,int type,int i);
	virtual ~MTObject() = 0;
	
	virtual bool MTCT islocked(){ return (lockread|lockwrite)!=0; };
	virtual bool MTCT ismodified() = 0;
	virtual bool MTCT lock(int flags,bool lock,int timeout = -1) = 0;
	virtual void MTCT setname(char *newname) = 0;
	virtual void MTCT setmodified(int value,int flags) = 0;
	virtual void MTCT notify(MTObject *source,int message,int param1,void *param2) = 0;
	virtual void MTCT enumchildren(MTObjectEnum enumproc,void *data) = 0;
	virtual int MTCT loadfromstream(MTFile *f,int size,void *params) = 0;
	virtual int MTCT savetostream(MTFile *f,void *params) = 0;
	virtual MTObject* MTCT duplicate(int targettype) = 0;
};
//---------------------------------------------------------------------------
#endif