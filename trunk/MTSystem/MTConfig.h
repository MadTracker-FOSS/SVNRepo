//---------------------------------------------------------------------------
//	$Id$
//---------------------------------------------------------------------------
#ifndef MTCONFIG_INCLUDED
#define MTCONFIG_INCLUDED
//---------------------------------------------------------------------------
enum MTConfigType{
	MTCT_CONFIG = 0,
	MTCT_SINTEGER,
	MTCT_UINTEGER,
	MTCT_FLOAT,
	MTCT_BOOLEAN,
	MTCT_STRING,
	MTCT_BINARY
};
//---------------------------------------------------------------------------
class MTConfigFile;
//---------------------------------------------------------------------------
#include "MTFile.h"
#include "MTStructures.h"
#include "../Headers/MTXExtension.h"
//---------------------------------------------------------------------------
class MTConfigFile{
public:
	MTConfigFile(const char *filename);
	virtual ~MTConfigFile();
	virtual void MTCT clear();
	virtual bool MTCT setsection(const char *name);
	virtual bool MTCT getparameter(const char *paramname,void *value,int desiredtype,int size);
	virtual bool MTCT createsection(const char *name);
	virtual bool MTCT setparameter(const char *paramname,void *value,int type,int size);
	virtual const char* MTCT getfilename();
private:
	MTFile *f;
	int sectionpos,sectionline,sectionnp,cpos,cline,cnp;
	MTArray *np;
};
//---------------------------------------------------------------------------
extern "C"
{
MTConfigFile* MTCT mtconfigfind(const char *filename);
MTConfigFile* MTCT mtconfigopen(const char *filename);
void MTCT mtconfigclose(MTConfigFile *file);
}
//---------------------------------------------------------------------------
#endif
