//---------------------------------------------------------------------------
//	$Id$
//---------------------------------------------------------------------------
#ifndef MTRESOURCES_INCLUDED
#define MTRESOURCES_INCLUDED
//---------------------------------------------------------------------------
#ifndef __BORLANDC__
#define MTR_WINDOW   'NWTM'
#define MTR_SKIN     'KSTM'
#define MTR_TEXT     'XTTM'
#define MTR_SHORTCUT 'CSTM'
#define MTR_BITMAP   'MBTM'
#define MTR_HTML     'THTM'
#define MTR_SAMPLE   'PSTM'
#else
#define MTR_WINDOW   'MTWN'
#define MTR_SKIN     'MTSK'
#define MTR_TEXT     'MTTX'
#define MTR_SHORTCUT 'MTSC'
#define MTR_BITMAP   'MTBM'
#define MTR_HTML     'MTHT'
#define MTR_SAMPLE   'MTSP'
#endif
//---------------------------------------------------------------------------
class MTResources;
//---------------------------------------------------------------------------
#include "MTFile.h"
#include "../Headers/MTXExtension.h"
//---------------------------------------------------------------------------
class MTResources{
public:
	MTResources(MTFile *f,bool ownfile);
	virtual ~MTResources();
	virtual int MTCT getnumresources();
	virtual bool MTCT getresourceinfo(int id,int *type,int *uid,int *size);
	virtual int MTCT loadresource(int type,int uid,void *buffer,int size);
	virtual int MTCT loadstring(int uid,char *buffer,int size);
	virtual int MTCT loadstringf(int uid,char *buffer,int size,...);
	virtual void* MTCT getresource(int type,int uid,int *size);
	virtual void MTCT releaseresource(void *res);
	virtual MTFile* MTCT getresourcefile(int type,int uid,int *size);
	virtual void MTCT releaseresourcefile(MTFile *f);
	virtual bool MTCT addresource(int type,int uid,void *res,int size);
	virtual bool MTCT addfile(int type,int uid,MTFile *f);
	virtual const char* MTCT getresourceurl();
private:
	struct MTResTable{
		int type;
		int uid;
		int offset;
		int size;
	} *table;
	MTFile *mf;
	int nres,onres,nares;
	bool modified;
	bool mownfile;
	void MTCT setmodified();
};
//---------------------------------------------------------------------------
extern "C"
{
MTResources* MTCT mtresfind(const char *filename,bool write);
MTResources* MTCT mtresopen(MTFile *f,bool ownfile);
void MTCT mtresclose(MTResources *res);
}
//---------------------------------------------------------------------------
#endif
