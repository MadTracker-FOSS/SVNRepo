//---------------------------------------------------------------------------
#ifndef MTAUDIO2_INCLUDED
#define MTAUDIO2_INCLUDED
//---------------------------------------------------------------------------
typedef void (__stdcall *RenderProc)(sample *source,void *dest,int count,int nchannels,int channel);
//---------------------------------------------------------------------------
void generateoutput();
//---------------------------------------------------------------------------
#endif
