
#include <windows.h>

#include "mcrt.h"


void HookText(void);

#ifndef USEGETCHARWIDTH
#ifdef __cplusplus
extern "C" {
#endif

// Macros to swap from Big Endian to Little Endian
#define SWAPWORD(x) MAKEWORD(HIBYTE(x), LOBYTE(x))
#define SWAPLONG(x) MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))

// Macro to pack a TrueType table name into a DWORD
#define MAKETABLENAME(ch1, ch2, ch3, ch4) \
	((((DWORD)(ch4)) << 24) | (((DWORD)(ch3)) << 16) | (((DWORD)(ch2)) << 8) | ((DWORD)(ch1)))

const DWORD dwCmapName = MAKETABLENAME('c','m','a','p');
const DWORD dwHeadName = MAKETABLENAME('h','e','a','d');
const DWORD dwHheaName = MAKETABLENAME('h','h','e','a');
const DWORD dwHmtxName = MAKETABLENAME('h','m','t','x');

#ifdef __cplusplus
}
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif
#pragma pack(push, 1)
struct jump_near
{
    BYTE opcode; // 0xe9
    DWORD relativeAddress;
};

struct const_str_adr
{
	DWORD stradr;
	LPCWSTR str;
};
#pragma pack(pop)
#ifdef __cplusplus
}
#endif