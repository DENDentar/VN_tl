
/*
SEHook: mcrt.cpp
Copyright (C) DENDentar.
*/

#include "mcrt.h"

#ifdef __cplusplus
extern "C" {
#endif

HANDLE MemHeap = NULL;
BOOL mcrtInitialized = FALSE;

// init CRT

void __cdecl memerror(int mode)
{
	wchar_t text[70];
	wsprintfW(text, L"%s\nError code: 0x%08x.", L"Memory allocation failure.", GetLastError());
	MessageBoxW(NULL, text, L"mcrt Error", MB_OK | MB_ICONERROR);
	if (mode == 0) ExitProcess(0);
}

void __cdecl initmcrt(void)
{
	if (mcrtInitialized && MemHeap) return;
#ifdef NEWHEAP
	if (MemHeap != NULL) 
		if (HeapDestroy(MemHeap) == FALSE) memerror(0);
	MemHeap = HeapCreate(NULL, 64*1024, 0);
#else
	MemHeap = GetProcessHeap();
#endif
	if (MemHeap == NULL) memerror(0);
	mcrtInitialized = TRUE;
}

void __cdecl deinitmcrt(void)
{
	if (!mcrtInitialized) return;
#ifdef NEWHEAP
	if (MemHeap != NULL) 
		if (HeapDestroy(MemHeap) == FALSE) memerror(0);
#endif
}
// end init CRT

// malloc
void* __cdecl memalloc(size_t size)
{
	LPVOID ret = HeapAlloc(MemHeap, HEAP_ZERO_MEMORY, size);
	if (ret == NULL) memerror(0);
	return ret;
}
// end new

// free
void __cdecl memfree(void *ptr)
{
	if (HeapFree(MemHeap, NULL, ptr) == FALSE) memerror(0);
}
// end delete/free

// memset
#pragma function(memset)
void * __cdecl memset(void *dst, int val, size_t count)
{
	void *start = dst;
	while (count--) {
		*(char *)dst = (char)val;
		dst = (char *)dst + 1;
	}
	return start;
}

// strlen
size_t __cdecl strlenA(const char *str)
{
	if (str == 0) return 0;
	size_t len = 0;
	while (*str++ != 0) len++;
	return len;
}

size_t __cdecl strlenW(const wchar_t *str)
{
	if (str == 0) return 0;
	size_t len = 0;
	while (*str++ != 0) len++;
	return len;
}
// end strlen

// strcpy
char* __cdecl strcpyA(char *destptr, const char *srcptr)
{
	//if (destptr == 0 || srcptr == 0) 
	char *ret = destptr;
	while (*srcptr != 0)
	{
		*destptr++ = *srcptr++;
	}
	++*destptr = 0;
	return ret;
}

wchar_t* __cdecl strcpyW(wchar_t *destptr, const wchar_t *srcptr)
{
	wchar_t *ret = destptr;
	while (*srcptr != 0)
	{
		*destptr++ = *srcptr++;
	}
	++*destptr = 0;
	return ret;
}
// end strcpy

// strncpy
char* __cdecl strncpyA(char *destptr, const char *srcptr, size_t num)
{
	char *ret = destptr;
	size_t num_tmp = 0;
	while (*srcptr != 0  && num_tmp < num)
	{
		*destptr++ = *srcptr++;
		num_tmp++;
	}
	++*destptr = 0;
	return ret;
}

wchar_t* __cdecl strncpyW(wchar_t *destptr, const wchar_t *srcptr, size_t num)
{
	wchar_t *ret = destptr;
	size_t num_tmp = 0;
	while (*srcptr != 0  && num_tmp < num)
	{
		*destptr++ = *srcptr++;
		num_tmp++;
	}
	++*destptr = 0;
	return ret;
}
// end strncpy

// strcmp
int __cdecl strcmpA(const char *str1, const char *str2)
{
	while(*str1 != 0 && *str2 != 0 && *str1 == *str2) { str1++; str2++; }
	return *str1 - *str2;
}

int __cdecl strcmpW(const wchar_t *str1, const wchar_t *str2)
{
	while(*str1 != 0 && *str2 != 0 && *str1 == *str2) { str1++; str2++; }
	return *str1 - *str2;
}
// end strcmp

// stricmp ASCII only
int __cdecl stricmpA(const char *str1, const char *str2)
{
	char str1char, str2char;
	while(*str1 != 0 && *str2 != 0)
	{
		str1char = *str1;
		str2char = *str2;
		if (str1char > 0x60 && str1char < 0x7b) str1char -= 0x20;
		if (str2char > 0x60 && str2char < 0x7b) str2char -= 0x20;
		if (str1char != str2char) break;
		str1++; str2++;
	}
	return *str1 - *str2;
}

int __cdecl stricmpW(const wchar_t *str1, const wchar_t *str2)
{
	wchar_t str1char, str2char;
	while(*str1 != 0 && *str2 != 0)
	{
		str1char = *str1;
		str2char = *str2;
		if (str1char > 0x60 && str1char < 0x7b) str1char -= 0x20;
		if (str2char > 0x60 && str2char < 0x7b) str2char -= 0x20;
		if (str1char != str2char) break;
		str1++; str2++;
	}
	return *str1 - *str2;
}
// end stricmp

#ifdef __cplusplus
}
#endif

// CRT end

void *operator new(size_t size) { return memalloc(size); }
void *operator new[](size_t size) { return memalloc(size); }

void operator delete(void *p) { memfree(p); }
void operator delete[](void *p) { memfree(p); }
