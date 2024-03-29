
/*
SEHook: mcrt.cpp
Copyright (C) DENDentar.
*/

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

// CRT start)
//extern HANDLE MemHeap;

// init CRT
void __cdecl initmcrt(void);
void __cdecl deinitmcrt(void);

// malloc
void* __cdecl memalloc(size_t size);

// free
void __cdecl memfree(void *ptr);

// memset
void * __cdecl memset(void *dst, int val, size_t count);
#pragma intrinsic(memset)

// strlen
size_t __cdecl strlenA(const char *str);

size_t __cdecl strlenW(const wchar_t *str);

// strcpy
char* __cdecl strcpyA(char *destptr, const char *srcptr);

wchar_t* __cdecl strcpyW(wchar_t *destptr, const wchar_t *srcptr);

// strncpy
char* __cdecl strncpyA(char *destptr, const char *srcptr, size_t num);

wchar_t* __cdecl strncpyW(wchar_t *destptr, const wchar_t *srcptr, size_t num);

// strcmp
int __cdecl strcmpA(const char *str1, const char *str2);

int __cdecl strcmpW(const wchar_t *str1, const wchar_t *str2);

// stricmp ASCII only
int __cdecl stricmpA(const char *str1, const char *str2);

int __cdecl stricmpW(const wchar_t *str1, const wchar_t *str2);

// CRT end

#ifdef __cplusplus
}
#endif

extern void *operator new(size_t size);
extern void *operator new[](size_t size);

extern void operator delete(void *р);
extern void operator delete[](void *р);