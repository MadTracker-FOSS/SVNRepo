//---------------------------------------------------------------------------
#ifndef MTBASE64_INCLUDED
#define MTBASE64_INCLUDED
//---------------------------------------------------------------------------
extern "C"
{
int mtbase64decode_len(const char *bufcoded);
int mtbase64decode(char *bufplain,const char *bufcoded);
int mtbase64decode_binary(unsigned char *bufplain,const char *bufcoded);
int mtbase64encode_len(int len);
int mtbase64encode(char *encoded,const char *string,int len);
int mtbase64encode_binary(char *encoded,const unsigned char *string,int len);
}
//---------------------------------------------------------------------------
#endif
