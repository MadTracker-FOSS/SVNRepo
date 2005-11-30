//---------------------------------------------------------------------------
//
//	MadTracker 3 Public Extension Header
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	http://www.madtracker.org/
//	info@madtracker.org
//
//	$Id$
//
//---------------------------------------------------------------------------
#ifndef MTXOBJECTS_INCLUDED
#define MTXOBJECTS_INCLUDED
//---------------------------------------------------------------------------
#include "MTXExtension.h"
//---------------------------------------------------------------------------
static const int objectstype = FOURCC('X','O','B','J');
//---------------------------------------------------------------------------
#include "MTXObject.h"
//---------------------------------------------------------------------------
struct MTObjectsPreferences{
	bool hexadecimal;
	bool showzeroes;
};

typedef bool (MTCT *ObjectIOFunc)(MTObject *object,char *filename,void *process);
typedef bool (MTCT *ObjectEditFunc)(MTObject *object,void *window,int flags,MTUser *user);

class ObjectType{
public:
	mt_uint32 type;
	char *description;
	virtual MTObject* MTCT create(MTObject *parent,mt_int32 id,void *param) = 0;
};

class MTObjectsInterface : public MTXInterface{
public:
	virtual MTObject* MTCT newobject(mt_uint32 type,MTObject *parent,mt_int32 id,void *param = 0,bool locked = false,bool assign = true) = 0;
	virtual void MTCT deleteobject(MTObject *object) = 0;
	virtual bool MTCT loadobject(MTObject *object,const char *filename,void *process = 0) = 0;
	virtual bool MTCT saveobject(MTObject *object,const char *filename,void *process = 0) = 0;
	virtual bool MTCT ownobject(MTObject *object,MTUser *user,bool silent = true) = 0;
	virtual void MTCT freeobject(MTObject *object,MTUser *user) = 0;
	virtual bool MTCT editobject(MTObject *object,void *window,int flags = 0) = 0;
	virtual void MTCT closeobject(MTObject *object) = 0;
	virtual int MTCT getnumtypes() = 0;
	virtual mt_uint32 MTCT gettype(int id) = 0;
	virtual ObjectType* MTCT getobjecttype(mt_uint32 type) = 0;
	virtual ObjectType* MTCT getobjecttype(const char *description) = 0;
	virtual mt_uint32 MTCT addobjecttype(ObjectType *type) = 0;
	virtual bool MTCT addload(mt_uint32 type,ObjectIOFunc loadfunc,const char *filetypes,const char *description) = 0;
	virtual bool MTCT addsave(mt_uint32 type,ObjectIOFunc savefunc,const char *filetypes,const char *description) = 0;
	virtual bool MTCT addedit(mt_uint32 type,ObjectEditFunc editfunc,const char *description) = 0;
	virtual void MTCT delobjecttype(ObjectType *type) = 0;
	virtual void MTCT delload(mt_uint32 type,ObjectIOFunc loadfunc) = 0;
	virtual void MTCT delsave(mt_uint32 type,ObjectIOFunc savefunc) = 0;
	virtual void MTCT deledit(mt_uint32 type,ObjectEditFunc editfunc) = 0;
};
//---------------------------------------------------------------------------
#endif
