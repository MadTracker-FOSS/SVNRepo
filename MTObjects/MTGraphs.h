//---------------------------------------------------------------------------
#ifndef MTGRAPHS_INCLUDED
#define MTGRAPHS_INCLUDED
//---------------------------------------------------------------------------
#include "../Headers/MTXControls.h"
//---------------------------------------------------------------------------
class MTCPUGraph : public MTCustomBehaviours{
public:
	MTCPUGraph(MTCustomControl *control);
	void MTCT ondestroy();
	void MTCT onsetbounds(int l,int t,int w,int h);
	void MTCT ondraw(MTRect &rect);
private:
	int refreshproc;
	int width,height;
	double scale;
	int lastid;
	double *history[2];
	static void MTCT cpuproc(void *cpu);
	void MTCT setcpu(double output,double objects);
};

class MTChannelsGraph : public MTCustomBehaviours{
public:
	MTChannelsGraph(MTCustomControl *control);
	void MTCT ondestroy();
	void MTCT onsetbounds(int l,int t,int w,int h);
	void MTCT ondraw(MTRect &rect);
private:
	int refreshproc;
	int width,height,scale;
	int lastid;
	int *history[2];
	static void MTCT chanproc(void *cpu);
	void MTCT setchannels(int foreground,int background);
};
//---------------------------------------------------------------------------
#endif
