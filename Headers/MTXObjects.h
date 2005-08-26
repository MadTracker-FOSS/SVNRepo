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
	int type;
	char *description;
	virtual MTObject* MTCT create(MTModule *parent,int id,void *param) = 0;
};

class MTObjectsInterface : public MTXInterface{
public:
	virtual MTObject* MTCT newobject(int type,MTModule *parent,int id,void *param = 0,bool locked = false,bool assign = true) = 0;
	virtual void MTCT deleteobject(MTObject *object) = 0;
	virtual bool MTCT loadobject(MTObject *object,const char *filename,void *process = 0) = 0;
	virtual bool MTCT saveobject(MTObject *object,const char *filename,void *process = 0) = 0;
	virtual bool MTCT ownobject(MTObject *object,MTUser *user,bool silent = true) = 0;
	virtual void MTCT freeobject(MTObject *object,MTUser *user) = 0;
	virtual bool MTCT editobject(MTObject *object,void *window,int flags = 0) = 0;
	virtual void MTCT closeobject(MTObject *object) = 0;
	virtual int MTCT getnumtypes() = 0;
	virtual int MTCT gettype(int id) = 0;
	virtual ObjectType* MTCT getobjecttype(int type) = 0;
	virtual ObjectType* MTCT getobjecttype(const char *description) = 0;
	virtual int MTCT addobjecttype(ObjectType *type) = 0;
	virtual bool MTCT addload(int type,ObjectIOFunc loadfunc,const char *filetypes,const char *description) = 0;
	virtual bool MTCT addsave(int type,ObjectIOFunc savefunc,const char *filetypes,const char *description) = 0;
	virtual bool MTCT addedit(int type,ObjectEditFunc editfunc,const char *description) = 0;
	virtual void MTCT delobjecttype(ObjectType *type) = 0;
	virtual void MTCT delload(int type,ObjectIOFunc loadfunc) = 0;
	virtual void MTCT delsave(int type,ObjectIOFunc savefunc) = 0;
	virtual void MTCT deledit(int type,ObjectEditFunc editfunc) = 0;
};
//---------------------------------------------------------------------------
#endif
