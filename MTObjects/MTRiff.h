//---------------------------------------------------------------------------
#ifndef MTRIFF_INCLUDED
#define MTRIFF_INCLUDED
//---------------------------------------------------------------------------
struct RIFFfmt{
	int fmtid;
	int fmtsize;
	short tag;
	short nchannels;
	int nsamplepersec;
	int nbytepersec;
	short nalign;
	short nbits;
};

struct RIFFdata{
	int dataid;
	int datasize;
};

struct RIFFWAVE{
	int waveid;
	RIFFfmt fmt;
};

struct RIFF{
	int riffid;
	int riffsize;
	RIFFWAVE wave;
	RIFFdata data;
};
//---------------------------------------------------------------------------
#endif