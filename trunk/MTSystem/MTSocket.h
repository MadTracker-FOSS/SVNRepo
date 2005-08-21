//---------------------------------------------------------------------------
//
//	MadTracker System Core
//
//		Platforms:	Win32
//		Processors: All
//
//	Copyright © 1999-2006 Yannick Delwiche. All rights reserved.
//
//	$Id$
//
//---------------------------------------------------------------------------
#ifndef MTSOCKET_INCLUDED
#define MTSOCKET_INCLUDED
//---------------------------------------------------------------------------
class MTSocket;
class MTServer;
class MTClient;
//---------------------------------------------------------------------------
#include "../Headers/MTXExtension.h"
#if defined(_WIN32)
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif
#include "MTSystem1.h"
//---------------------------------------------------------------------------
class MTSocket{
public:
	MTSocket(bool datagram = false);
	MTSocket(MTServer *cserver,SOCKET cs,sockaddr_in *caddr);
	~MTSocket();
	bool isdatagram;
	virtual bool MTCT isconnected();
	virtual void MTCT disconnect();
	virtual int MTCT getlasterror();
	virtual int MTCT read(void *buffer,int size,sockaddr *from = 0,int *fromsize = 0);
	virtual int MTCT write(const void *buffer,int size);
	virtual void MTCT setblocking(bool b);
	virtual const char* MTCT getname();
	virtual int MTCT getip();
	virtual int MTCT getendip();
protected:
	MTServer *server;
	bool connected;
	int ip;
	int port;
	int lasterror;
	SOCKET s;
	sockaddr_in addr;
	int MTCT makesocket();
};

class MTServer : public MTSocket{
public:
	MTServer(int p,int max,bool datagram = false);
	~MTServer();
	virtual int MTCT write(const void *buffer,int size);
	virtual MTSocket* MTCT accept();
	virtual void MTCT disconnectclient(MTSocket *s);
	virtual int MTCT getnumclients();
	virtual MTSocket* MTCT getclient(int id);
private:
	int maxconn;
	int nclients;
	MTSocket **clients;
};

class MTClient : public MTSocket{
public:
	MTClient(const char *a,int p,bool datagram = false);
	MTClient(int ip,int p,bool datagram = false);
	virtual bool MTCT reconnect();
};
//---------------------------------------------------------------------------
extern "C"
{
void initSocket();
void uninitSocket();
}
//---------------------------------------------------------------------------
extern int (WSAAPI *wsgetlasterror)();
extern SOCKET (WSAAPI *mtaccept)(SOCKET,struct sockaddr*,int*);
extern int (WSAAPI *mtbind)(SOCKET,const struct sockaddr*,int);
extern int (WSAAPI *mtclosesocket)(SOCKET);
extern int (WSAAPI *mtconnect)(SOCKET,const struct sockaddr*,int);
extern struct hostent* (WSAAPI *mtgethostbyaddr)(const char*,int,int);
extern struct hostent* (WSAAPI *mtgethostbyname)(const char*);
extern int (WSAAPI *mtgethostname)(char*,int);
extern u_short (WSAAPI *mthtons)(u_short);
extern unsigned long (WSAAPI *mtinet_addr)(const char*);
extern char* (WSAAPI *mtinet_ntoa)(struct in_addr);
extern int (WSAAPI *mtioctlsocket)(SOCKET,long,u_long*);
extern int (WSAAPI *mtsetsockopt)(SOCKET,int,int,const char*,int);
extern int (WSAAPI *mtlisten)(SOCKET,int);
extern int (WSAAPI *mtrecv)(SOCKET,char*,int,int);
extern int (WSAAPI *mtsend)(SOCKET,const char*,int,int);
extern int (WSAAPI *mtrecvfrom)(SOCKET s, char*,int,int,struct sockaddr*,int*);
extern int (WSAAPI *mtsendto)(SOCKET,const char*,int,int,const struct sockaddr*,int);
extern int (WSAAPI *mtshutdown)(SOCKET,int);
extern int (WSAAPI *mtsocket)(int,int,int);
//---------------------------------------------------------------------------
#endif
