
/*
SEHook: Shookapi.h
Copyright (C) DENDentar.
*/

#ifdef APICALLLOG
#define APICALLOGCLEAR(file) if (true) \
	{ \
	CreateDirectoryW(L"ApiCallLog", NULL); \
	/*HANDLE hFile = CreateFileW(file, GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, NULL, NULL);*/ \
	HANDLE hFile = CreateFileW(file, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, NULL, NULL); \
	DWORD Written = 0; \
	SYSTEMTIME st; \
	GetLocalTime(&st); \
	SetFilePointer(hFile, NULL, NULL, FILE_END); \
	wchar_t str[128]; \
	wsprintfW(str, L"\r\n\r\n---- Log start: %d.%02d.%04d %d:%02d ----\r\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute); \
	WriteFile(hFile, str, wcslen(str)*2, &Written, NULL); \
	CloseHandle(hFile); \
} else (void)0
#endif

// ---- Kernel32 ----
bool __stdcall HookUser32(void);
// ------------------

// ---- Kernel32 ----

#ifdef APICALLLOG
#define APICALLOGWRITEKER32(arg) if (true) \
	{ wchar_t *ApiLogStr; \
		ApiLogStr = new wchar_t[512]; \
		wsprintfW arg; \
		ApiCallLogWrite(L"ApiCallLog\\Kernel32CallLog.txt", ApiLogStr); \
		delete[] ApiLogStr; \
	} else (void)0
#endif

bool __stdcall HookKernel32(void);

// ------------------


// ---- Version -----

#ifdef APICALLLOG
#define APICALLOGWRITEVER(arg) if (true) \
	{ wchar_t *ApiLogStrVer; \
		ApiLogStrVer = new wchar_t[512]; \
		wsprintfW arg; \
		ApiCallLogWrite(L"ApiCallLog\\VersionCallLog.txt", ApiLogStrVer); \
		delete[] ApiLogStrVer; \
	} else (void)0
#endif

bool __stdcall HookVersion(void);

// -----------------

// ----- Gdi32 -----

bool __stdcall HookGDI32(void);

// -----------------

// ------ Dbghelp.dll -------
#if (SLVER >= 0x01000400 && SLVER <= 0x0100070A)
bool __stdcall HookDbghelp(void);
#endif
// --------------------------
