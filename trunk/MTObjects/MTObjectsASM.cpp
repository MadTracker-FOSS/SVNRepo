//---------------------------------------------------------------------------
//
//	MadTracker Objects
//
//		Platforms:	All
//		Processors:	x86
//
//	Copyright © 1999-2003 Yannick Delwiche. All rights reserved.
//
//---------------------------------------------------------------------------
#include "MTObjectsASM.h"
//---------------------------------------------------------------------------
bool MTACT a_floattofixed(double f,int &i,unsigned int &d)
{
	__asm{
		push	esi
		mov		eax,dword ptr [f+4]
		push	edi
		test	eax,0x7F800000
		mov		edx,dword ptr [f]
		jnz		_ok
		xor		eax,eax
		xor		edx,edx
		jmp		_exit
_ok:
		push	eax
		mov		ecx,eax
		shr		ecx,20
		and		eax,0xFFFFF
		sub		ecx,0x3FF+20
		or		eax,0x100000
		neg		cl
		cmp		cl,32
		jae		_s32
		shrd	edx,eax,cl
		shr		eax,cl
		jmp		_sign
_s32:
		mov		edx,eax
		sub		cl,32
		xor		eax,eax
		shr		edx,cl
_sign:
		mov		ecx,eax
		pop		eax
		bt		eax,31
		sbb		eax,eax
		and		eax,1
_exit:
		mov		esi,i
		mov		edi,d
		mov		[esi],ecx
		mov		[edi],edx
		pop		edi
		pop		esi
	};
}
//---------------------------------------------------------------------------
void MTACT a_changesign(char *buffer,int count)
{
	_asm{
		push	esi
		push	edi
		mov		esi,buffer
		mov		ecx,count
		mov		edi,esi
		mov		edx,ecx
		shr		ecx,2

_loop:
		lodsd
		xor		eax,0x80808080
		dec		ecx
		stosd
		jnz		_loop
		and		edx,0x3
		jz		_exit
_loop2:
		lodsb
		xor		al,0x80
		dec		edx
		stosb
		jnz		_loop2

_exit:
		pop		edi
		pop		esi
	};
}
//---------------------------------------------------------------------------
void MTACT a_delta_add_8(char *dest,char *source,int count)
{
	_asm{
		push	esi
		push	edi
		mov		esi,source
		mov		edi,dest
		mov		ecx,count
		dec		edi
_add:
		inc		edi
		lodsb
		add		[edi],al
		dec		ecx
		jnz		_add

		pop		edi
		pop		esi
	};
}
//---------------------------------------------------------------------------
void MTACT a_delta_add_16(short *dest,short *source,int count)
{
	_asm{
		push	esi
		push	edi
		mov		esi,source
		mov		edi,dest
		mov		ecx,count
		sub		edi,2
_add:
		add		edi,2
		lodsw
		add		[edi],ax
		dec		ecx
		jnz		_add

		pop		edi
		pop		esi
	};
}
//---------------------------------------------------------------------------
void MTACT a_delta_encode_8(char *buffer,int count)
{
	_asm{
	};
}
//---------------------------------------------------------------------------
void MTACT a_delta_encode_16(short *buffer,int count)
{
	_asm{
	};
}
//---------------------------------------------------------------------------
void MTACT a_delta_decode_8(char *buffer,int count)
{
	_asm{
		push	esi
		push	edi
		mov		esi,buffer
		mov		ecx,count
		mov		edi,esi
		xor		edx,edx
		dec		edi

_decode:
		lodsb
		inc		edi
		add		dl,al
		mov		[edi],dl
		dec		ecx
		mov		al,dl
		jnz		_decode

		pop		edi
		pop		esi
	};
}
//---------------------------------------------------------------------------
void MTACT a_delta_decode_16(short *buffer,int count)
{
	_asm{
		push	esi
		push	edi
		mov		esi,buffer
		mov		ecx,count
		mov		edi,esi
		xor		edx,edx
		sub		edi,2

_decode:
		lodsw
		add		edi,2
		add		dx,ax
		mov		[edi],dx
		dec		ecx
		mov		ax,dx
		jnz		_decode

		pop		edi
		pop		esi
	};
}
//---------------------------------------------------------------------------
void MTACT a_deinterleave_8(char **dest,char *source,int channels,int count)
{
	_asm{
		push	esi
		push	edi
		push	ebx
		mov		edx,channels
		dec		edx
		jl		_exit
_nextchan:
		mov		esi,source
		lea		esi,[esi+edx]
		mov		ebx,dest
		mov		edi,[ebx+edx*2]
		mov		ecx,count
		mov		ebx,channels
		dec		ebx
_loop:
		lodsb
		add		esi,ebx
		stosb
		dec		ecx
		jnz		_loop
		dec		edx
		jge		_nextchan
_exit:
		pop		ebx
		pop		edi
		pop		esi
	};
}
//---------------------------------------------------------------------------
void MTACT a_deinterleave_16(short **dest,short *source,int channels,int count)
{
	_asm{
		push	esi
		push	edi
		push	ebx
		mov		edx,channels
		dec		edx
		jl		_exit
_nextchan:
		mov		esi,source
		lea		esi,[esi+edx*2]
		mov		ebx,dest
		mov		edi,[ebx+edx*4]
		mov		ecx,count
		mov		ebx,channels
		dec		ebx
		shl		ebx,1
_loop:
		lodsw
		add		esi,ebx
		stosw
		dec		ecx
		jnz		_loop
		dec		edx
		jge		_nextchan
_exit:
		pop		ebx
		pop		edi
		pop		esi
	};
}
//---------------------------------------------------------------------------
void MTACT a_calcposition(int &posi,unsigned int &posd,int pitch,unsigned int pitchd,int count,bool reverse)
{
	_asm{
		push	ebx
		mov		ebx,posi
		mov		ecx,posd
		xor		edx,edx
		mov		eax,pitchd
		mul		count
		push	eax
		push	edx
		xor		edx,edx
		mov		eax,pitch
		mul		count
		add		[esp],eax
		pop		edx
		pop		eax
		cmp		reverse,0
		jne		_reverse
		add		[ecx],eax
		adc		[ebx],edx
		jmp		_exit
_reverse:
		sub		[ecx],eax
		sbb		[ebx],edx
_exit:
		pop		ebx
	};
}
//---------------------------------------------------------------------------
