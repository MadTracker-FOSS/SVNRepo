//---------------------------------------------------------------------------
#ifndef MTDSP1_INCLUDED
#define MTDSP1_INCLUDED

#ifdef __BORLANDC__
static const int dsptype = 'XDSP';
#else
static const int dsptype = 'PSDX';
#endif

#define RESAMPLE_8BIT   0x00
#define RESAMPLE_16BIT  0x01
#define RESAMPLE_32BIT  0x02

#define RESAMPLE_NONE   0x00
#define RESAMPLE_LINEAR 0x10
#define RESAMPLE_CUBIC1 0x20
#define RESAMPLE_CUBIC2 0x30
#define RESAMPLE_SINC1  0x40
#define RESAMPLE_SINC2  0x50
#define RESAMPLE_SINC3  0x60

#define FILTER_INIT 0x1
#define FILTER_RAMP 0x2
//---------------------------------------------------------------------------
struct ChannelStatus;
struct FilterStatus;
struct DSPFunctions;
class MTDSPInterface;
//---------------------------------------------------------------------------
#include "../Headers/MTXExtension.h"
#include "../Headers/MTXSystem.h"
//---------------------------------------------------------------------------
struct ChannelStatus{
	int posi;
	unsigned int posd;
	double vol;
	double volvar;
	int volvarlng;
	double multiplier;
	unsigned char tc;
	unsigned char sc;
	bool reverse;
};

struct FilterStatus{
	int flags;
	int fvarlng,rvarlng;
	double c,r;
	double cv,rv;
	double t[4];
	float frequency;
	float resonance;
	float frequency2;
	float resonance2;
};

typedef void (__stdcall *MTResampleProc)(sample *dest,sample *source,int count,ChannelStatus &status,int pitchi,unsigned int pitchd);

class MTDSPInterface : public MTXInterface{
public:
	void (MTACT *emptybuffer)(sample *dest,int count);
	void (MTACT *replacebuffer)(sample *dest,sample *source,int count);
	void (MTACT *replacebuffermul)(sample *dest,sample *source,double mul,int count);
	void (MTACT *addbuffer)(sample *dest,sample *source,int count);
	void (MTACT *addbuffermul)(sample *dest,sample *source,double mul,int count);
	void (MTACT *addbuffermul2)(sample *dest1,sample *dest2,sample *source,double mul1,double mul2,int count);
	void (MTACT *addbufferslide)(sample *dest,sample *source,double mul,double inc,int count);
	void (MTACT *addbufferslide2)(sample *dest1,sample *dest2,sample *source,double mul1,double mul2,double inc1,double inc2,int count);
	void (MTACT *ampbuffer)(sample *dest,double mul,int count);
	void (MTACT *filter[16])(sample *dest,sample *source,FilterStatus &status,int count,int frequency);
	void (MTACT *resample[32])(char *dest,char *source,int count,ChannelStatus &status,int pitchi,unsigned int pitchd);

	MTDSPInterface();
	bool MTCT init();
	void MTCT uninit();
	void MTCT start();
	void MTCT stop();
	void MTCT processcmdline(void *params);
	void MTCT showusage(void *out);
	int MTCT config(int command,int param);
};
//---------------------------------------------------------------------------
extern MTInterface *mtinterface;
extern MTDSPInterface *oi;
extern MTSystemInterface *si;
//---------------------------------------------------------------------------
#endif
