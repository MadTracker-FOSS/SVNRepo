//---------------------------------------------------------------------------
//
//	MadTracker Console
//
//		Platforms:	All
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id$
//
//---------------------------------------------------------------------------
#include "MTConsole.h"
#include "MTExtensions.h"
//---------------------------------------------------------------------------
MTConsole::MTConsole(MTInterface *mti)
{
}

MTConsole::MTConsole(MTFile *parent,int start,int end,int access)
{
}

MTConsole::~MTConsole()
{
}

int MTConsole::read(void *buffer,int size)
{
	int read = 0;

	return read;
}

int MTConsole::readln(char *buffer,int maxsize)
{
	int read = 0;

	return read;
}
/*
int MTConsole::reads(char *buffer,int maxsize)
{
	int read = 0;

	return read;
}
*/
int MTConsole::write(const void *buffer,int size)
{
	size = 0;

	return size;
}

int MTConsole::seek(int pos,int origin)
{
	return -1;
}

int MTConsole::length()
{
	return -1;
}

void* MTConsole::getpointer(int offset,int size)
{
	return 0;
}

void MTConsole::releasepointer(void *mem)
{
}

int MTConsole::pos()
{
	return -1;
}

bool MTConsole::eof()
{
	return false;
}

bool MTConsole::seteof()
{
	return false;
}

bool MTConsole::gettime(int *modified,int *accessed)
{
	return false;
}

bool MTConsole::settime(int *modified,int *accessed)
{
	return false;
}

MTFile* MTConsole::subclass(int start,int length,int access)
{
	return 0;
}

int MTConsole::userinput(const char *input)
{
	int x,y;

	for (x=0;x<next;x++){
		MTExtension &cext = *ext[x];
		if (!cext.system){
			for (y=0;y<cext.i->ninterfaces;y++){
				MTTRY{
					if (cext.i->interfaces[y]->status & MTX_INITIALIZED){
						cext.i->interfaces[y]->processinput(input);
					};
				}
				MTCATCH{
					LOGD("%s - ERROR: Exception while processing input in '");
					LOG(cext.i->interfaces[y]->name);
					LOG("'!"NL);
				};
			};
		};
	};
	return 0;
}
//---------------------------------------------------------------------------
